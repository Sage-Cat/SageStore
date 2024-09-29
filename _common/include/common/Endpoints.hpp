#pragma once

namespace Endpoints {
namespace Segments {
inline constexpr int ROOT        = 0;
inline constexpr int MODULE      = 1;
inline constexpr int SUBMODULE   = 2;
inline constexpr int RESOURCE_ID = 3;
}; // namespace Segments

namespace Users {
inline constexpr char LOGIN[] = "/api/users/login";
inline constexpr char USERS[] = "/api/users/users";
inline constexpr char ROLES[] = "/api/users/roles";
} // namespace Users
} // namespace Endpoints
