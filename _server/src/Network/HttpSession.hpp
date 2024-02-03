#pragma once

#include <memory>
#include <functional>
#include <vector>

#include "NetworkCommon.hpp"

class BusinessLogicFacade;
class IDataSerializer;
class ResponseData;

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession(tcp::socket socket,
                //  ssl::context &ctx,
                std::unique_ptr<IDataSerializer> serializer,
                BusinessLogicFacade &facade); // Add BusinessLogicFacade reference

    void start();

private:
    void do_read();
    void on_read(beast::error_code ec, std::size_t bytes_transferred);
    void on_write(beast::error_code ec, std::size_t bytes_transferred, bool close);
    void do_close();
    void handle_request();
    void do_response(const ResponseData &data);

private:
    std::unique_ptr<IDataSerializer> m_serializer;

    // beast::ssl_stream<beast::tcp_stream> m_stream;
    beast::tcp_stream m_stream; // will be just tcp for now
    
    beast::flat_buffer m_buffer;
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;
    BusinessLogicFacade &m_businessLogicFacade; // Business logic facade reference
};
