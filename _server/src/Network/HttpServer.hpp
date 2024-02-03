#pragma once

#include "NetworkCommon.hpp"

class BusinessLogicFacade;

class HttpServer
{
public:
    HttpServer(const std::string &address, unsigned short port, BusinessLogicFacade &businessLogicFacade);

    void run();

private:
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);

private:
    net::io_context m_ioc;
    // ssl::context m_ssl_context; // Without SSL for now
    tcp::acceptor m_acceptor;
    BusinessLogicFacade &m_businessLogicFacade;
};
