#include "HttpServer.hpp"

#include "HttpSession.hpp"
#include "JsonSerializer.hpp"
#include "BusinessLogic/BusinessLogicFacade.hpp"

#include "SpdlogConfig.hpp"

HttpServer::HttpServer(const std::string &address, unsigned short port, BusinessLogicFacade &businessLogicFacade)
    : m_ioc{1},
      /* m_ssl_context{ssl::context::tlsv12_server}, */
      m_acceptor{m_ioc, tcp::endpoint{net::ip::make_address(address), static_cast<net::ip::port_type>(port)}},
      m_businessLogicFacade(businessLogicFacade)
{
    SPDLOG_TRACE("HttpServer::HttpServer");
    SPDLOG_INFO("HttpServer set listen to address: {}, port: {}", address, port);
    try
    {
        // m_ssl_context.use_certificate_chain_file(NetworkCommon::CERTIFICATE_FILE);
        // m_ssl_context.use_private_key_file(NetworkCommon::KEY_FILE, ssl::context::pem);
        // m_ssl_context.set_verify_mode(ssl::verify_none);

        do_accept();
    }
    catch (const std::exception &e)
    {
        SPDLOG_ERROR("HttpServer Constructor Exception: {}", e.what());
    }
}

void HttpServer::run()
{
    SPDLOG_TRACE("HttpServer::run");
    m_ioc.run();
}

void HttpServer::do_accept()
{
    SPDLOG_TRACE("HttpServer::do_accept");
    m_acceptor.async_accept(
        [this](beast::error_code ec, tcp::socket socket)
        {
            on_accept(std::move(ec), std::move(socket));
        });
}

void HttpServer::on_accept(beast::error_code ec, tcp::socket socket)
{
    SPDLOG_TRACE("HttpServer::on_accept");
    if (!ec)
    {
        std::make_shared<HttpSession>(
            std::move(socket),
            // m_ssl_context,
            std::make_unique<JsonSerializer>(),
            m_businessLogicFacade)
            ->start(); // start the session
    }
    do_accept();
}