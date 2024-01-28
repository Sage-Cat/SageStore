#pragma once

#include <QString>

namespace Api
{
    namespace Endpoints
    {
        const QString LOGIN = "/users/login";
        const QString REGISTER = "/users/register";
    }

    namespace Params
    {
        const QString TOKEN = "token";
        const QString ENDPOINT = "endpoint";
        const QString ERROR = "error";
    }
}