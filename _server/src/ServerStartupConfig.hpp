#pragma once

#include <string>

namespace ServerStartupConfig {

std::string envOrDefault(const char *name, const char *fallback);
unsigned short envOrDefaultPort(const char *name, unsigned short fallback);

} // namespace ServerStartupConfig
