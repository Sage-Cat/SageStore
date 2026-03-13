#include <gtest/gtest.h>

#include <cstdlib>
#include <string>

#include "ServerStartupConfig.hpp"

namespace {

class ScopedEnvVar {
public:
    ScopedEnvVar(const char *name, const char *value) : m_name(name)
    {
        const char *existingValue = std::getenv(name);
        if (existingValue != nullptr) {
            m_hadValue = true;
            m_previousValue = existingValue;
        }

        set(value);
    }

    ~ScopedEnvVar()
    {
        if (m_hadValue) {
            set(m_previousValue.c_str());
        } else {
            unset();
        }
    }

private:
    void set(const char *value)
    {
#ifdef _WIN32
        _putenv_s(m_name.c_str(), value);
#else
        setenv(m_name.c_str(), value, 1);
#endif
    }

    void unset()
    {
#ifdef _WIN32
        _putenv_s(m_name.c_str(), "");
#else
        unsetenv(m_name.c_str());
#endif
    }

    std::string m_name;
    bool m_hadValue{false};
    std::string m_previousValue;
};

} // namespace

TEST(ServerStartupConfigTest, EnvOrDefaultPortReturnsFallbackWhenVariableMissing)
{
    ScopedEnvVar env("SAGESTORE_SERVER_PORT_TEST", "");
    EXPECT_EQ(ServerStartupConfig::envOrDefaultPort("SAGESTORE_SERVER_PORT_TEST", 8001), 8001);
}

TEST(ServerStartupConfigTest, EnvOrDefaultPortReturnsParsedValueForValidPort)
{
    ScopedEnvVar env("SAGESTORE_SERVER_PORT_TEST", "18081");
    EXPECT_EQ(ServerStartupConfig::envOrDefaultPort("SAGESTORE_SERVER_PORT_TEST", 8001), 18081);
}

TEST(ServerStartupConfigTest, EnvOrDefaultPortReturnsFallbackForNegativePort)
{
    ScopedEnvVar env("SAGESTORE_SERVER_PORT_TEST", "-1");
    EXPECT_EQ(ServerStartupConfig::envOrDefaultPort("SAGESTORE_SERVER_PORT_TEST", 8001), 8001);
}

TEST(ServerStartupConfigTest, EnvOrDefaultPortReturnsFallbackForTooLargePort)
{
    ScopedEnvVar env("SAGESTORE_SERVER_PORT_TEST", "70000");
    EXPECT_EQ(ServerStartupConfig::envOrDefaultPort("SAGESTORE_SERVER_PORT_TEST", 8001), 8001);
}

TEST(ServerStartupConfigTest, EnvOrDefaultPortReturnsFallbackForNonNumericPort)
{
    ScopedEnvVar env("SAGESTORE_SERVER_PORT_TEST", "not-a-port");
    EXPECT_EQ(ServerStartupConfig::envOrDefaultPort("SAGESTORE_SERVER_PORT_TEST", 8001), 8001);
}

TEST(ServerStartupConfigTest, EnvOrDefaultPortReturnsFallbackForTrailingJunk)
{
    ScopedEnvVar env("SAGESTORE_SERVER_PORT_TEST", "18081junk");
    EXPECT_EQ(ServerStartupConfig::envOrDefaultPort("SAGESTORE_SERVER_PORT_TEST", 8001), 8001);
}
