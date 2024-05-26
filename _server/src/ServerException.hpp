#pragma once

#include <exception>
#include <string>

class ServerException : public std::exception {
public:
    explicit ServerException(const std::string &componentName, const std::string &msg);

    virtual const char *what() const noexcept override;

    const char *componentName() const noexcept;

private:
    std::string m_componentName, m_message;
};
