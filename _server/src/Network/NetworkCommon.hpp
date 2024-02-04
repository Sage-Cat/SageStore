#pragma once

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

/**
 * @namespace NetworkCommon
 * @brief Defines common network configuration constants and utilities for server and client communication.
 *
 * This namespace encapsulates common settings and paths used for network operations, especially those related to security
 * (e.g., SSL/TLS certificates and keys), even though the SSL specifics are currently commented out to simplify the example.
 */
namespace NetworkCommon
{
    const std::string CERTIFICATE_FILE = ""; ///< Path to the SSL certificate file
    const std::string KEY_FILE = "";         ///< Path to the SSL private key file
}
