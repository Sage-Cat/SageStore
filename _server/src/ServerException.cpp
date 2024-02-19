#include "ServerException.hpp"

ServerException::ServerException(const std::string &componentName, const std::string &msg)
    : m_componentName(componentName),
      m_message(msg) {}

const char *ServerException::what() const noexcept
{
    return m_message.c_str();
}

const char *ServerException::componentName() const noexcept
{
    return m_componentName.c_str();
}
