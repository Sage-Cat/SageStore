#pragma once

#include <memory>

#include "NetworkCommon.hpp"

class BusinessLogicFacade;
class IDataSerializer;
class ResponseData;

/**
 * @class HttpSession
 * @brief Manages an individual HTTP session, handling requests and responses over a TCP socket.
 *
 * This class encapsulates the logic for reading from and writing to an HTTP connection, parsing requests,
 * dispatching them to the appropriate business logic, and serializing responses back to the client.
 */
class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
    /**
     * @brief Constructs an HttpSession with a session ID, TCP socket, data serializer, and a reference to the BusinessLogicFacade.
     *
     * @param session_id Unique identifier for the session.
     * @param socket TCP socket for communication.
     * @param serializer Unique pointer to an IDataSerializer for data serialization and deserialization.
     * @param facade Reference to BusinessLogicFacade for processing business logic.
     */
    HttpSession(unsigned long long session_id,
                tcp::socket socket,
                std::unique_ptr<IDataSerializer> serializer,
                BusinessLogicFacade &facade);

    /**
     * @brief Starts the session, initiating reading from the socket.
     */
    void start();

private:
    /**
     * @brief Reads data from the socket asynchronously.
     */
    void do_read();

    /**
     * @brief Callback for handling data read from the socket.
     *
     * @param ec Error code object for identifying errors.
     * @param bytes_transferred Number of bytes read from the socket.
     */
    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    /**
     * @brief Callback for handling completion of data writing to the socket.
     *
     * @param ec Error code object for identifying errors.
     * @param bytes_transferred Number of bytes written to the socket.
     * @param close Flag indicating whether to close the session after writing.
     */
    void on_write(beast::error_code ec, std::size_t bytes_transferred, bool close);

    /**
     * @brief Closes the session and cleans up resources.
     */
    void do_close();

    /**
     * @brief Handles the request, dispatching it to the appropriate business logic processing.
     */
    void handle_request();

    /**
     * @brief Sends a response back to the client.
     *
     * @param data The ResponseData object containing the response to be serialized and sent.
     */
    void do_response(const ResponseData &data);

private:
    std::unique_ptr<IDataSerializer> m_serializer; ///< Serializer for request/response data.

    beast::tcp_stream m_stream; ///< TCP stream for socket communication.

    beast::flat_buffer m_buffer;                   ///< Buffer for reading from the stream.
    http::request<http::dynamic_body> m_request;   ///< HTTP request object.
    http::response<http::dynamic_body> m_response; ///< HTTP response object.

    BusinessLogicFacade &m_businessLogicFacade; ///< Reference to the business logic processing facade.

    const unsigned long long m_sessionId; ///< Unique session identifier.
};
