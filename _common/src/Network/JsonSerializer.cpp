#include "common/Network/JsonSerializer.hpp"

#include "common/SpdlogConfig.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string JsonSerializer::serialize(const Dataset &dataset)
{
    SPDLOG_TRACE("JsonSerializer::serialize");

    json jsonObject;

    if (dataset.empty()) {
        return json({}).dump();
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
        jsonObject[key] = jsonRow;
    }

    return jsonObject.dump();
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
