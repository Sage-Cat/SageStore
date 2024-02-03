#pragma once

#include <boost/beast.hpp>
#include <boost/asio.hpp>
// #include <boost/asio/ssl.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
// namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

namespace NetworkCommon
{
    const std::string CERTIFICATE_FILE = "";
    const std::string KEY_FILE = "";
}