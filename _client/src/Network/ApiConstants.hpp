#pragma once

#include <QString>

/**
 * @namespace Api
 * @brief Defines constants for interacting with the application's backend API.
 *
 * This namespace includes the base API URL, specific endpoints for user-related actions, and common parameter names used in API requests and responses.
 */
namespace Api
{
    /// Base URL for the API.
    const QString API_URL{"http://127.0.0.1:8001/api"};

    /**
     * @namespace Api::Endpoints
     * @brief Contains namespaces for specific API endpoint groupings.
     */
    namespace Endpoints
    {
        /**
         * @namespace Api::Endpoints::Users
         * @brief Defines endpoints related to Users module such as login and registration.
         */
        namespace Users
        {
            /// Prefix for user-related API endpoints.
            const QString USERS_PREFIX{"/users"};

            /// Endpoint for user login action.
            const QString LOGIN = USERS_PREFIX + "/login";

            /// Endpoint for user registration action.
            const QString REGISTER = USERS_PREFIX + "/register";

            /// Endpoint for user role action
            const QString GET_ROLE = USERS_PREFIX + "/users/roles";

            /// Endpoint for post new role
            const QString NEW_ROLE = USERS_PREFIX + "/users/roles";

            /// Endpoint for editing role
            const QString EDIT_ROlE = USERS_PREFIX + "/users/roles";

            /// Endpoint for deleting role
            const QString DELETE_ROlE = USERS_PREFIX + "/users/roles";
        }
    }

    /**
     * @namespace Api::Params
     * @brief Defines common parameter names used in API requests and responses.
     */
    namespace Params
    {
        /// Parameter name for authentication token.
        const QString TOKEN{"token"};

        /// Parameter name for specifying the API endpoint in a request.
        const QString ENDPOINT{"endpoint"};

        /// Parameter name for error messages in API responses. Note: "ERROR" is a reserved word in Qt.
        const QString ERR{"error"};
    }
}
