#pragma once

#include "NetworkCommon.hpp"
#include "HttpTransaction.hpp"
#include "DataSpecs.hpp"

/**
 * @class HttpServer
 * @brief Implements an HTTP server that accepts connections and manages HTTP sessions.
 *
 * This class sets up a listening TCP socket on a specified address and port. It accepts incoming
 * connections and creates HttpTransaction objects for handling each connection, leveraging the
 * BusinessLogicFacade for processing requests.
 */
class HttpServer
{
public:
    using ExecuteBusinessTaskCallback = std::function<void(RequestData, BusinessLogicCallback)>;

    /**
     * @brief Constructs an HttpServer with a given address, port, and reference to BusinessLogicFacade.
     *
     * @param address The IP address on which the server will listen.
     * @param port The port number on which the server will listen.
     * @param callback to execute business layer task
     */
    HttpServer(const std::string &address, unsigned short port, ExecuteBusinessTaskCallback callback);

    /**
     * @brief Destructor for HttpServer.
     */
    ~HttpServer();

    /**
     * @brief Starts the server, making it listen for incoming connections and process requests.
     */
    void run();

private:
    /**
     * @brief Asynchronously accepts incoming connections.
     */
    void do_accept();

    /**
     * @brief Callback for when a new connection is accepted.
     *
     * @param ec Error code indicating the result of the accept operation.
     * @param socket The TCP socket representing the accepted connection.
     */
    void on_accept(beast::error_code ec, tcp::socket socket);

private:
    net::io_context m_ioc; ///< The I/O context used to perform asynchronous operations.
    tcp::acceptor m_acceptor; ///< Acceptor used for listening for and accepting incoming connections.

    unsigned long long transactionId{0}; ///< Counter for generating unique transaction IDs.

    ExecuteBusinessTaskCallback m_executeBusinessTaskCallback; // Store the callback};
};
