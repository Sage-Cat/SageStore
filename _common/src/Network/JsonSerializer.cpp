#include "common/Network/JsonSerializer.hpp"

<<<<<<< HEAD
#include "common/SpdlogConfig.hpp"
#include <nlohmann/json.hpp>
=======
#include <nlohmann/json.hpp>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
>>>>>>> 099b94d (Implement Base Frontend for Users Management)

using json = nlohmann::json;

// Utility function to remove invalid UTF-8 bytes
std::string remove_invalid_utf8(const std::string &str)
{
    std::string result;
    result.reserve(str.size());
    for (auto it = str.begin(); it != str.end(); ++it) {
        unsigned char c = static_cast<unsigned char>(*it);
        if (c < 0x80) {
            result.push_back(c);
        } else if ((c & 0xE0) == 0xC0) {
            if (it + 1 != str.end() && (static_cast<unsigned char>(*(it + 1)) & 0xC0) == 0x80) {
                result.push_back(c);
                result.push_back(*(it + 1));
                ++it;
            }
        } else if ((c & 0xF0) == 0xE0) {
            if (it + 2 != str.end() && (static_cast<unsigned char>(*(it + 1)) & 0xC0) == 0x80 &&
                (static_cast<unsigned char>(*(it + 2)) & 0xC0) == 0x80) {
                result.push_back(c);
                result.push_back(*(it + 1));
                result.push_back(*(it + 2));
                it += 2;
            }
        } else if ((c & 0xF8) == 0xF0) {
            if (it + 3 != str.end() && (static_cast<unsigned char>(*(it + 1)) & 0xC0) == 0x80 &&
                (static_cast<unsigned char>(*(it + 2)) & 0xC0) == 0x80 &&
                (static_cast<unsigned char>(*(it + 3)) & 0xC0) == 0x80) {
                result.push_back(c);
                result.push_back(*(it + 1));
                result.push_back(*(it + 2));
                result.push_back(*(it + 3));
                it += 3;
            }
        }
    }
    return result;
}

std::string JsonSerializer::serialize(const Dataset &dataset)
{
    SPDLOG_TRACE("JsonSerializer::serialize");

    json jsonObject;

    if (dataset.empty()) {
        return jsonObject.dump();
    }

    for (const auto &[key, value] : dataset) {
        json jsonRow = json::array();
        for (const auto &cell : value) {
            try {
                jsonRow.push_back(cell);
            } catch (const nlohmann::json::exception &e) {
                SPDLOG_WARN("Skipping invalid UTF-8 character in key: {}. Error: {}", key,
                            e.what());
                continue;
            }
        }
        return jsonObject.dump();
    } catch (const json::exception &e) {
        SPDLOG_ERROR("JSON serialization failed: {}", e.what());
        return "{}";
    }
}

Dataset JsonSerializer::deserialize(const std::string &serializedData)
{
    SPDLOG_TRACE("JsonSerializer::deserialize");

    Dataset dataset;
    try {
        auto jsonObject = json::parse(serializedData, nullptr, false);

        if (!jsonObject.is_object()) {
            SPDLOG_ERROR("Invalid JSON document");
            return dataset;
        }

        for (auto &[key, jsonRow] : jsonObject.items()) {
            if (!jsonRow.is_array()) {
                SPDLOG_ERROR("Invalid JSON array for key: {}", key);
                continue;
            }

            Data row;
            for (auto &cellValue : jsonRow) {
                try {
                    row.push_back(cellValue.get<std::string>());
                } catch (const nlohmann::json::exception &e) {
                    SPDLOG_WARN("Skipping invalid UTF-8 character in key: {}. Error: {}", key,
                                e.what());
                    continue;
                }
            }
            dataset[key] = std::move(row);
        }
    } catch (const nlohmann::json::exception &e) {
        SPDLOG_ERROR("Error parsing JSON: {}", e.what());
    }

    return dataset;
}