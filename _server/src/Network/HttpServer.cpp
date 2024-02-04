#include "HttpServer.hpp"

#include "HttpSession.hpp"
#include "JsonSerializer.hpp"
#include "BusinessLogic/BusinessLogicFacade.hpp"

#include "SpdlogConfig.hpp"

HttpServer::HttpServer(const std::string &address, unsigned short port, BusinessLogicFacade &businessLogicFacade)
    : m_ioc{1},
      m_acceptor{m_ioc, tcp::endpoint{net::ip::make_address(address), static_cast<net::ip::port_type>(port)}},
      m_businessLogicFacade(businessLogicFacade)
{
    SPDLOG_TRACE("HttpServer::HttpServer");
    SPDLOG_INFO("HttpServer set listen to address: {}, port: {}", address, port);
    try
    {
        do_accept();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("HttpServer Constructor Exception: {}", e.what());
    }
}

HttpServer::~HttpServer()
{
}

void HttpServer::run()
{
    SPDLOG_TRACE("HttpServer::run");
    m_ioc.run();
}

void HttpServer::do_accept()
{
    m_acceptor.async_accept([this](beast::error_code ec, tcp::socket socket)
                            { on_accept(std::move(ec), std::move(socket)); });
}

void HttpServer::on_accept(beast::error_code ec, tcp::socket socket)
{
    if (!ec)
    {
        const auto sessionId = nextSessionId++;
        auto session = std::make_shared<HttpSession>(
            sessionId,
            std::move(socket),
            std::make_unique<JsonSerializer>(),
            m_businessLogicFacade);
        session->start();
        sessions[sessionId] = std::move(session);

        SPDLOG_INFO("Session created with ID: {}", sessionId);
    }
    do_accept();
}