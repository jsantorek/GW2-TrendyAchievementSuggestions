// clang-format off
#include <winsock2.h>
// clang-format on
#include "EfficiencyTrends.hpp"
#include <FallbackData.hpp>
#include <Nexus.h>
#include <httplib.h>
#include <limits>
#include <nlohmann/json.hpp>

namespace G
{
extern AddonAPI *APIDefs;
}

std::tuple<std::string, std::unordered_map<std::string, int>> FetchTimestampedData()
{
    try
    {
        httplib::Client cli("http://edge.gw2efficiency.com");
        auto res = cli.Get("/tracking/unlocks?v=2021-07-24T00%3A00%3A00Z&id=achievements");
        if (res->status != 200)
            throw std::runtime_error(res->body);
        auto json = nlohmann::json::parse(res->body);
        return {json["updatedAt"].template get<std::string>(),
                json["data"].template get<std::unordered_map<std::string, int>>()};
    }
    catch (...)
    {
        G::APIDefs->Log(ELogLevel_WARNING, ADDON_NAME, "Failed to fetch latest data, using embeded fallback data");
    }
    auto json = nlohmann::json::parse(UNLOCKS_DATA);
    return {json["updatedAt"].template get<std::string>(),
            json["data"].template get<std::unordered_map<std::string, int>>()};
}

uint32_t EfficiencyTrends::GetAchievementPopularity(uint32_t ID)
{
    auto it = Raw.find(ID);
    if (it != Raw.end())
        return it->second;
    return 0;
}

void EfficiencyTrends::PrioritizeAdventureGuide()
{
    auto priority = (std::numeric_limits<uint32_t>::max)();
    auto volumeOne = {6600, 6601, 6614, 6615, 6620, 6622, 6635, 6636, 6637,
                      6638, 6646, 6647, 6659, 6662, 6663, 6669, 6678};
    auto volumeTwo = {6581, 6582, 6593, 6594, 6602, 6611, 6612, 6613, 6621, 6625, 6628, 6631,
                      6634, 6657, 6660, 6664, 6666, 6670, 6672, 6727, 6731, 6733, 6741};
    auto volumeThree = {6580, 6583, 6585, 6604, 6605, 6606, 6607, 6608, 6618,
                        6630, 6639, 6665, 6667, 6675, 6715, 6744, 6746};
    auto volumeFour = {6588, 6595, 6596, 6597, 6609, 6610, 6617, 6623, 6626, 6640, 6641, 6642,
                       6643, 6648, 6650, 6653, 6668, 6671, 6676, 6677, 6712, 6721, 6725};
    auto volumeFive = {6584, 6587, 6589, 6591, 6616, 6624, 6627, 6629, 6645, 6649, 6651, 6654, 6656, 6661, 6681, 6734};
    for (auto volumeIds : {volumeOne, volumeTwo, volumeThree, volumeFour, volumeFive})
    {
        for (const auto id : volumeIds)
            Raw[id] = priority;
        priority--;
    }
}

void EfficiencyTrends::Refresh()
{
    auto [timestamp, data] = FetchTimestampedData();
    Raw.clear();
    for (auto [id, count] : data)
        Raw.emplace(std::stoi(id), count);
    G::APIDefs->Log(ELogLevel_INFO, ADDON_NAME, std::format("Using GW2Efficiency data from {}", timestamp).c_str());
    if (AdventureGuidePrioritized)
        PrioritizeAdventureGuide();
}
