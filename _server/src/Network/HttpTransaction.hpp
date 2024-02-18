#pragma once

#include <memory>

#include "NetworkCommon.hpp"
#include "DataSpecs.hpp"

class IDataSerializer;

/**
 * @class HttpTransaction
 * @brief Manages an individual HTTP transaction, handling requests and responses over a TCP socket.
 *
 * This class encapsulates the logic for reading from and writing to an HTTP connection, parsing requests,
 * dispatching them to the appropriate business logic, and serializing responses back to the client.
 */
class HttpTransaction : public std::enable_shared_from_this<HttpTransaction>
{
public:
    using PostBusinessTaskCallback = std::function<void(RequestData, BusinessLogicCallback)>;

    /**
     * @brief Constructs an HttpTransaction with a transaction ID, TCP socket, data serializer, and a reference to the BusinessLogicFacade.
     *
     * @param session_id Unique identifier for the transaction.
     * @param socket TCP socket for communication.
     * @param serializer Unique pointer to an IDataSerializer for data serialization and deserialization.
     * @param callback to post business task
     */
    HttpTransaction(unsigned long long id,
                    tcp::socket socket,
                    std::unique_ptr<IDataSerializer> serializer,
                    PostBusinessTaskCallback callback);

    ~HttpTransaction();

    /**
     * @brief Starts the transaction, initiating reading from the socket.
     */
    void start();

private:
    /**
     * @brief Reads data from the socket asynchronously.
     */
    void do_read();

    /**
     * @brief Handles the request, dispatching it to the appropriate business logic processing.
     */
    void handle_request();

    /**
     * @brief Sends a response back to the client.
     *
     * @param data The ResponseData object containing the response to be serialized and sent.
     */
    void do_response(ResponseData data);

    /**
     * @brief Closes the transaction and cleans up resources.
     */
    void do_close();

private:
    const unsigned long long m_id;                       ///< Unique transaction identifier.
    beast::tcp_stream m_stream;                          ///< TCP stream for socket communication.
    std::unique_ptr<IDataSerializer> m_serializer;       ///< Serializer for request/response data.
    PostBusinessTaskCallback m_postBusinessTaskCallback; ///< Callback to post business task

    beast::flat_buffer m_buffer;                   ///< Buffer for reading from the stream.
    http::request<http::dynamic_body> m_request;   ///< HTTP request object.
    http::response<http::dynamic_body> m_response; ///< HTTP response object.
};
