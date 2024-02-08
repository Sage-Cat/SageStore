// HttpSession.cpp
#include "HttpSession.hpp"

#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "IDataSerializer.hpp"
#include "BusinessLogic/BusinessLogicFacade.hpp"

#include <spdlog/spdlog.h>

HttpSession::HttpSession(unsigned long long session_id,
                         tcp::socket socket,
                         std::unique_ptr<IDataSerializer> serializer,
                         BusinessLogicFacade &facade)
    : m_sessionId(session_id),
      m_stream(std::move(socket) /*, ctx*/),
      m_serializer(std::move(serializer)),
      m_businessLogicFacade(facade)
{
    SPDLOG_TRACE("HttpSession::HttpSession");
}

void HttpSession::start()
{
    SPDLOG_TRACE("HttpSession::start");

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
    SPDLOG_INFO("Session ID: {} received a request", m_sessionId);

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
        SPDLOG_DEBUG("Received data (before deserialization): {}", bodyStr);
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
        SPDLOG_ERROR("HttpSession::handle_request error: this is not API requets, expected /api endpoint");
        return;
    }

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
    SPDLOG_INFO("Processing request for session ID: {}", m_sessionId);
    m_businessLogicFacade.executeTask(requestData, callback);
}

void HttpSession::do_response(const ResponseData &data)
{
    SPDLOG_TRACE("HttpSession::do_response");

    m_response.result(http::status::ok);
    m_response.set(http::field::content_type, "application/json");
    const auto serializedData = m_serializer->serialize(data.dataset);

    SPDLOG_DEBUG("Sending data (serialized): {}", serializedData);
    beast::ostream(m_response.body()) << serializedData;

    SPDLOG_INFO("Session ID: {} sent response to client", m_sessionId);

    auto self = shared_from_this();
    http::async_write(m_stream, m_response,
                      [self](beast::error_code ec, std::size_t bytes_transferred)
                      {
                          self->on_write(ec, bytes_transferred, ec == http::error::end_of_stream);
                      });
}
