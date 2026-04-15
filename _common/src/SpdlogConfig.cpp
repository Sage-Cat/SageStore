#include "common/SpdlogConfig.hpp"

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <spdlog/logger.h>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace {
constexpr const char *kLogDirectoryEnv = "SAGESTORE_LOG_DIR";
constexpr const char *kDefaultPattern =
    "[%Y-%m-%d %H:%M:%S.%e] [%n] [pid:%P] [tid:%t] [%^%l%$] %v";

std::mutex &loggerMutex()
{
    static std::mutex mutex;
    return mutex;
}

std::string envOrEmpty(const char *name)
{
    const char *value = std::getenv(name);
    if (value == nullptr) {
        return {};
    }
    return value;
}

std::filesystem::path resolveLogDirectory(const SpdlogConfig::Options &options)
{
    if (!options.logDirectory.empty()) {
        return std::filesystem::path(options.logDirectory);
    }

    const std::string envDirectory = envOrEmpty(kLogDirectoryEnv);
    if (!envDirectory.empty()) {
        return std::filesystem::path(envDirectory);
    }

    return std::filesystem::current_path() / "logs";
}

std::string sanitizeLoggerName(const std::string &loggerName)
{
    return loggerName.empty() ? std::string("SageStore") : loggerName;
}

std::shared_ptr<spdlog::logger> createNullLogger(const std::string &loggerName)
{
    auto sink   = std::make_shared<spdlog::sinks::null_sink_mt>();
    auto logger = std::make_shared<spdlog::logger>(sanitizeLoggerName(loggerName), sink);
    logger->set_pattern(kDefaultPattern);
    logger->set_level(spdlog::level::off);
    logger->flush_on(spdlog::level::off);
    return logger;
}

std::shared_ptr<spdlog::logger>
createLogger(spdlog::level::level_enum level, const SpdlogConfig::Options &options,
             std::string &startupNotice)
{
    using SinkPtr = std::shared_ptr<spdlog::sinks::sink>;

    std::vector<SinkPtr> sinks;
    startupNotice.clear();

    if (options.enableConsole) {
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    if (options.enableFile) {
        try {
            const std::filesystem::path logDirectory = resolveLogDirectory(options);
            std::error_code errorCode;
            std::filesystem::create_directories(logDirectory, errorCode);
            if (errorCode) {
                throw std::filesystem::filesystem_error(
                    "create_directories", logDirectory, errorCode);
            }

            const std::filesystem::path logFilePath =
                logDirectory /
                (options.logFileStem.empty() ? "sagestore.log"
                                             : options.logFileStem + ".log");

            sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                logFilePath.string(), options.maxFileSizeBytes, options.maxFileCount, false));
            startupNotice = "File logging enabled at " + logFilePath.string();
        } catch (const std::exception &exception) {
            startupNotice =
                "File logging disabled; falling back without file sink: " +
                std::string(exception.what());
        }
    }

    if (sinks.empty()) {
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        if (startupNotice.empty()) {
            startupNotice = "No active sinks configured; falling back to console logging";
        }
    }

    auto logger =
        std::make_shared<spdlog::logger>(sanitizeLoggerName(options.loggerName),
                                         sinks.begin(), sinks.end());
    logger->set_pattern(kDefaultPattern);
    logger->set_level(level);
    logger->flush_on(spdlog::level::warn);
    return logger;
}
} // namespace

namespace SpdlogConfig {
void initialize(spdlog::level::level_enum level, const Options &options)
{
    std::scoped_lock lock(loggerMutex());

    if (level == spdlog::level::off) {
        spdlog::set_default_logger(createNullLogger(options.loggerName));
        spdlog::set_level(spdlog::level::off);
        return;
    }

    std::string startupNotice;
    auto logger = createLogger(level, options, startupNotice);
    spdlog::set_default_logger(logger);
    spdlog::set_level(level);
    spdlog::flush_on(spdlog::level::warn);

    if (!startupNotice.empty()) {
        SPDLOG_INFO("{}", startupNotice);
    }
}

void shutdown()
{
    std::scoped_lock lock(loggerMutex());
    if (const auto logger = spdlog::default_logger(); logger != nullptr) {
        logger->flush();
    }
}
} // namespace SpdlogConfig
