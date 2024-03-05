#pragma once

namespace Endpoints
{
    namespace Segments
    {
        inline constexpr int ROOT = 0;
        inline constexpr int MODULE = 1;
        inline constexpr int SUBMODULE = 2;
        inline constexpr int RESOURCE_ID = 3;
    };

    namespace Users
    {
        inline constexpr char USERS_PREFIX[] = "/users";
        inline constexpr char LOGIN[] = "/users/login";
        inline constexpr char REGISTER[] = "/users/register";
        inline constexpr char ROLES[] = "/users/roles";
    }
}
