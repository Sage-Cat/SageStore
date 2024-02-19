#include "HttpServer.hpp"

#include <boost/bind/bind.hpp>

#include "JsonSerializer.hpp"

#include "SpdlogConfig.hpp"

HttpServer::HttpServer(const std::string &address, unsigned short port, ExecuteBusinessTaskCallback callback)
    : m_ioc{1},
      m_acceptor{m_ioc, tcp::endpoint{asio::ip::make_address(address), static_cast<asio::ip::port_type>(port)}},
      m_executeBusinessTaskCallback(std::move(callback))
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
    SPDLOG_TRACE("HttpServer::~HttpServer");
}

void HttpServer::run()
{
    SPDLOG_TRACE("HttpServer::run");
    m_ioc.run();
}

void HttpServer::do_accept()
{
    SPDLOG_TRACE("HttpServer::do_accept");
    m_acceptor.async_accept([this](beast::error_code ec, tcp::socket socket)
                            { on_accept(std::move(ec), std::move(socket)); });
}

void HttpServer::on_accept(beast::error_code ec, tcp::socket socket)
{
    SPDLOG_TRACE("HttpServer::on_accept");
    if (!ec)
    {
        auto transaction = std::make_shared<HttpTransaction>(
            transactionId,
            std::move(socket),
            std::make_unique<JsonSerializer>(),
            [this](RequestData requestData, BusinessLogicCallback callback) // TODO: use m_ioc.wrap() instead
            {
                auto boundTask = boost::bind(m_executeBusinessTaskCallback, std::move(requestData), std::move(callback));
                asio::post(m_ioc, boundTask);
            });
        transaction->start();
        SPDLOG_INFO("Transaction created with ID: {}", transactionId);
        transactionId++;
    }
    do_accept();
}
