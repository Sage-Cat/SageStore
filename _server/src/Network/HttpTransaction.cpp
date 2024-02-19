#include "HttpTransaction.hpp"

#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>

#include "IDataSerializer.hpp"

#include "SpdlogConfig.hpp"

HttpTransaction::HttpTransaction(unsigned long long id,
                                 boost::asio::ip::tcp::socket socket,
                                 std::unique_ptr<IDataSerializer> serializer,
                                 PostBusinessTaskCallback callback)
    : m_id(id),
      m_stream(std::move(socket)),
      m_serializer(std::move(serializer)),
      m_postBusinessTaskCallback(std::move(callback))
{
    SPDLOG_TRACE("[Transaction ID: {}] - HttpTransaction::HttpTransaction initiated", m_id);
}

HttpTransaction::~HttpTransaction()
{
    SPDLOG_TRACE("[Transaction ID: {}] - HttpTransaction::~HttpTransaction object was destroyed", m_id);
}

void HttpTransaction::start()
{
    SPDLOG_TRACE("[Transaction ID: {}] - HttpTransaction::start", m_id);
    do_read();
}

void HttpTransaction::do_read()
{
    SPDLOG_TRACE("[Transaction ID: {}] - HttpTransaction::do_read", m_id);
    m_buffer.consume(m_buffer.size());
    m_request = {};

    http::async_read(m_stream, m_buffer, m_request,
                     [self = shared_from_this()](boost::beast::error_code ec, std::size_t)
                     {
                         if (!ec)
                         {
                             self->handle_request();
                         }
                         else
                         {
                             SPDLOG_ERROR("[Transaction ID: {}] - HttpTransaction::do_read::async_read failed with error: {}", self->m_id, ec.message());
                         }
                     });
}

void HttpTransaction::handle_request()
{
    SPDLOG_TRACE("[Transaction ID: {}] - HttpTransaction::handle_request", m_id);
    SPDLOG_INFO("[Transaction ID: {}] - Received a request", m_id);

    // Parse request
    const auto target = std::string(m_request.target());
    std::vector<std::string> segments;
    boost::split(segments, target, boost::is_any_of("/"), boost::token_compress_on);

    // Trunc all ""
    segments.erase(std::remove_if(segments.begin(), segments.end(),
                                  [](const std::string &s)
                                  { return s.empty(); }),
                   segments.end());

    if (m_request.body().size() != 0)
    {
        auto bodyStr = beast::buffers_to_string(m_request.body().data());
        SPDLOG_DEBUG("[Transaction ID: {}] SERVER received data: {}", m_id, bodyStr);
    }

    // Prepare RequestData
    RequestData requestData;
    requestData.method = std::string(m_request.method_string());
    const std::string root = segments[0];
    requestData.module = segments[1];
    requestData.submodule = segments[2];
    requestData.dataset = m_serializer->deserialize(
        beast::buffers_to_string(m_request.body().data()));

    if (root != "api")
    {
        SPDLOG_ERROR("[Transaction ID: {}] - HttpSession::handle_request error: Not an API request, expected /api endpoint", m_id);
        return;
    }

    if (requestData.module.empty() || requestData.submodule.empty())
    {
        SPDLOG_ERROR("[Transaction ID: {}] - HttpSession::handle_request error: Module or submodule not specified", m_id);
        return;
    }

    BusinessLogicCallback callback = [self = shared_from_this()](ResponseData responseData)
    {
        self->do_response(std::move(responseData));
    };

    // Post some task to be executed
    m_postBusinessTaskCallback(std::move(requestData), std::move(callback));
}

void HttpTransaction::do_response(ResponseData data)
{
    SPDLOG_TRACE("[Transaction ID: {}] - HttpTransaction::do_response", m_id);

    m_response.result(http::status::ok);
    m_response.set(http::field::content_type, "application/json");
    const auto serializedData = m_serializer->serialize(data.dataset);

    beast::ostream(m_response.body()) << serializedData;

    SPDLOG_DEBUG("[Transaction ID: {}] SERVER sent data: {}", m_id, serializedData);

    // Ensure the response is fully sent before closing the transaction
    auto self = shared_from_this();
    http::async_write(m_stream, m_response,
                      [self](boost::beast::error_code ec, std::size_t)
                      {
                          if (!ec)
                          {
                              self->do_close();
                          }
                          else
                          {
                              SPDLOG_ERROR("[Transaction ID: {}] - HttpTransaction::async_write error: {}", self->m_id, ec.message());
                          }
                      });
}

void HttpTransaction::do_close()
{
    SPDLOG_TRACE("HttpTransaction::do_close");
    boost::beast::error_code ec;
    m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    if (ec)
    {
        SPDLOG_ERROR("[Transaction ID: {}] - Shutdown error: {}", m_id, ec.message());
    }
    // Transaction is automatically restored by itself without Server participation
}