#pragma once

#include <QString>

/**
 * @namespace Api
 * @brief Defines constants for interacting with the application's backend API.
 */
namespace Endpoints
{
    /**
     * @namespace Api::Endpoints::Users
     */
    namespace Users
    {
        const QString USERS_PREFIX{"/users"};
        const QString LOGIN = USERS_PREFIX + "/login";
        const QString REGISTER = USERS_PREFIX + "/register";
    }
}
