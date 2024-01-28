#pragma once

#include <QString>

namespace Api
{
    const QString API_URL = "http://localhost:8000/api/";

    namespace Endpoints
    {
        const QString LOGIN = "/users/login";
        const QString REGISTER = "/users/register";
    }

    namespace Params
    {
        const QString TOKEN = "token";
        const QString ENDPOINT = "endpoint";
        const QString ERR = "error"; // ERROR is defined in Qt
    }
}