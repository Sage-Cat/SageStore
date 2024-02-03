// HttpSession.cpp
#include "HttpSession.hpp"

#include <boost/algorithm/string.hpp>

#include "IDataSerializer.hpp"
#include "BusinessLogic/BusinessLogicFacade.hpp"

#include "SpdlogConfig.hpp"

HttpSession::HttpSession(tcp::socket socket,
                         //    ssl::context &ctx,
                         std::unique_ptr<IDataSerializer> serializer,
                         BusinessLogicFacade &facade)
    : m_stream(std::move(socket) /*, ctx*/), m_serializer(std::move(serializer)), m_businessLogicFacade(facade)
{
    SPDLOG_TRACE("HttpSession::HttpSession");
}

void HttpSession::start()
{
    SPDLOG_TRACE("HttpSession::start");
    // Perform the SSL handshake
    // m_stream.async_handshake(ssl::m_streambase::server,
    //                          [self = shared_from_this()](beast::error_code ec)
    //                          {
    //                              if (!ec)
    //                              {
    //                                  self->do_read();
    //                              }
    //                              else
    //                              {
    //                                  SPDLOG_ERROR("HttpSession::start::async_handshake" + ec.to_string());
    //                              }
    //                          });

    do_read();
}

void HttpSession::do_read()
{
    SPDLOG_TRACE("HttpSession::do_read");

    // Clear the buffer and request
    m_buffer.consume(m_buffer.size());
    m_request = {};

    // Read a request
    http::async_read(m_stream, m_buffer, m_request,
                     [self = shared_from_this()](beast::error_code ec, std::size_t bytes_transferred)
                     {
                         if (!ec)
                         {
                             self->handle_request();
                         }
                         else
                         {
                             SPDLOG_ERROR("HttpSession::do_read::async_read" + ec.to_string());
                         }
                     });
}

void HttpSession::on_write(beast::error_code ec, std::size_t bytes_transferred, bool close)
{
    SPDLOG_TRACE("HttpSession::on_write");
    if (ec)
    {
        SPDLOG_ERROR("HttpSession::on_write ERROR: " + ec.to_string());
        return;
    }

    if (close)
    {
        SPDLOG_INFO("HttpSession::on_write connection will be closed");
        return do_close();
    }

    // Read another request
    do_read();
}

void HttpSession::do_close()
{
    SPDLOG_TRACE("HttpSession::do_close");

    // Perform the SSL shutdown
    // m_stream.async_shutdown(
    //     [self = shared_from_this()](beast::error_code ec)
    //     {
    //         self->m_stream.lowest_layer().shutdown(tcp::socket::shutdown_send, ec);
    //     });

    beast::error_code ec;
    m_stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    if (ec)
    {
        SPDLOG_ERROR("Shutdown error: {}", ec.message());
    }
}

void HttpSession::handle_request()
{
    SPDLOG_TRACE("HttpSession::handle_request");

    // Parse request
    const auto target = std::string(m_request.target());
    std::vector<std::string> segments;
    boost::split(segments, target, boost::is_any_of("/"), boost::token_compress_on);

    // Prepare RequestData
    RequestData requestData;
    requestData.method = std::string(m_request.method_string());
    requestData.module = segments.size() > 0 ? segments[0] : "";
    requestData.submodule = segments.size() > 1 ? segments[1] : "";
    requestData.dataset = m_serializer->deserialize(
        beast::buffers_to_string(m_request.body().data()));

    if (requestData.module.empty() || requestData.submodule.empty())
    {
        SPDLOG_ERROR("HttpSession::handle_request error: module or submodule not specified");
        return;
    }

    // Prepare callback for business layer
    BusinessLogicCallback callback = [this](const ResponseData &responseData)
    {
        this->do_response(responseData);
    };

    // Process request and send response
    m_businessLogicFacade.executeTask(requestData, callback);
}

void HttpSession::do_response(const ResponseData &data)
{
    m_response.result(http::status::ok);
    m_response.set(http::field::content_type, "application/json");
    const auto serializedData = m_serializer->serialize(data.dataset);
    beast::ostream(m_response.body()) << serializedData;
    m_response.prepare_payload();

    auto self = shared_from_this();
    http::async_write(m_stream, m_response,
                      [self](beast::error_code ec, std::size_t bytes_transferred)
                      {
                          self->on_write(ec, bytes_transferred, ec == http::error::end_of_stream);
                      });
}
