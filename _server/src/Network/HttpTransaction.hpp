#pragma once

#include <memory>

#include "NetworkGeneral.hpp"
#include "DataSpecs.hpp"

class IDataSerializer;

/**
 * @class HttpTransaction
 * @brief Manages an individual HTTP transaction, handling requests and responses over a TCP socket.
 */
class HttpTransaction : public std::enable_shared_from_this<HttpTransaction>
{
public:
    using PostBusinessTaskCallback = std::function<void(RequestData, BusinessLogicCallback)>;

    HttpTransaction(unsigned long long id,
                    tcp::socket socket,
                    std::unique_ptr<IDataSerializer> serializer,
                    PostBusinessTaskCallback callback);

    ~HttpTransaction();

    void start();

private:
    void do_read();
    void handle_request();
    void do_response(ResponseData data);
    void do_close();

private:
    std::vector<std::string> parseEndpoint(const std::string &endpoint) const;

private:
    const unsigned long long m_id;
    beast::tcp_stream m_stream;
    std::unique_ptr<IDataSerializer> m_serializer;
    PostBusinessTaskCallback m_postBusinessTaskCallback;

    beast::flat_buffer m_buffer;
    http::request<http::dynamic_body> m_request;
    http::response<http::dynamic_body> m_response;
};
