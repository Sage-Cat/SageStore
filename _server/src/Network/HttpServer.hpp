#pragma once

#include "NetworkCommon.hpp"

class HttpSession;
class BusinessLogicFacade;

/**
 * @class HttpServer
 * @brief Implements an HTTP server that accepts connections and manages HTTP sessions.
 *
 * This class sets up a listening TCP socket on a specified address and port. It accepts incoming
 * connections and creates HttpSession objects for handling each connection, leveraging the
 * BusinessLogicFacade for processing requests.
 */
class HttpServer
{
public:
    /**
     * @brief Constructs an HttpServer with a given address, port, and reference to BusinessLogicFacade.
     *
     * @param address The IP address on which the server will listen.
     * @param port The port number on which the server will listen.
     * @param businessLogicFacade Reference to the BusinessLogicFacade for request processing.
     */
    HttpServer(const std::string &address, unsigned short port, BusinessLogicFacade &businessLogicFacade);

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

    unsigned long long nextSessionId{0};                                           ///< Counter for generating unique session IDs.
    std::unordered_map<unsigned long long, std::shared_ptr<HttpSession>> sessions; ///< Map of session IDs to HttpSession objects.

    BusinessLogicFacade &m_businessLogicFacade; ///< Reference to the business logic layer for processing requests.
};
