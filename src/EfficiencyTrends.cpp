// clang-format off
#include <winsock2.h>
// clang-format on
#include "EfficiencyTrends.hpp"
#include "Game/Achievement/AchCategoryDef.h"
#include "Game/Achievement/AchievementDef.h"
#include "Game/Content/ContentContext.h"
#include "Game/Content/EContentType.h"
#include "Game/PropContext.h"
#include "Game/Types.h"
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
    GW2RE::CContentCtx ctx = GW2RE::CPropContext::Get()->ContentCtx;
    if (!ctx)
    {
        G::APIDefs->Log(ELogLevel_WARNING, ADDON_NAME,
                        "Content context missing - unable to find Character Adventure Guide achievements");
        return;
    }
    const auto CharacterAdventureGuideGroup = GW2RE::GUID_t("EFADEE67-588F-412F-A1BD-6C9AFF782988");
    auto cat = ctx.GetContentStream<GW2RE::AchievementCategoryDef_t>(GW2RE::EContentType::AchievementCategoryDef);
    while (auto c = cat.next())
    {
        if (!c->Group || c->Group->Content.GUID != CharacterAdventureGuideGroup)
            continue;
        const auto priority = (std::numeric_limits<uint32_t>::max)() - c->Ordering;
        for (auto ach : *c->Achievements)
        {
            Raw[ach->ID] = priority;
        }
        G::APIDefs->Log(
            ELogLevel_INFO, ADDON_NAME,
            std::format("Character Adventure Guide category #{} found, {} achievements priority set to 0x{:08X}", c->ID,
                        c->Achievements->Count, priority)
                .c_str());
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
