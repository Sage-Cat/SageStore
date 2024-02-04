#include "JsonSerializer.hpp"
#include "SpdlogConfig.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string JsonSerializer::serialize(const Dataset &dataset)
{
    SPDLOG_TRACE("JsonSerializer::serialize");

    json jsonObject;
    for (const auto &[key, value] : dataset)
    {
        json jsonRow = json::array();
        for (const auto &cell : value)
        {
            jsonRow.push_back(cell);
        }
        jsonObject[key] = jsonRow;
    }

    return jsonObject.dump();
}

Dataset JsonSerializer::deserialize(const std::string &serializedData)
{
    SPDLOG_TRACE("JsonSerializer::deserialize");

    Dataset dataset;
    auto jsonObject = json::parse(serializedData, nullptr, false); // parse and get a json object

    if (!jsonObject.is_object())
    {
        SPDLOG_ERROR("Invalid JSON document");
        return dataset;
    }

    for (auto &[key, jsonRow] : jsonObject.items())
    {
        if (!jsonRow.is_array())
        {
            SPDLOG_ERROR("Invalid JSON array for key: {}", key);
            continue;
        }

        Data row;
        for (auto &cellValue : jsonRow)
        {
            row.push_back(cellValue.get<std::string>());
        }
        dataset[key] = std::move(row);
    }

    return dataset;
}
