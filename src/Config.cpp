#include "Config.hpp"
#include "ConfigurableExclusions.hpp"
#include "EfficiencyTrends.hpp"
#include "Hooks.hpp"
#include <Nexus.h>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <string>

namespace G
{
extern AddonAPI *APIDefs;
} // namespace G

NLOHMANN_JSON_SERIALIZE_ENUM(SeasonalAchievementsHandling,
                             {
                                 {SeasonalAchievementsHandling::AlwaysExclude, "AlwaysExclude"},
                                 {SeasonalAchievementsHandling::AlwaysInclude, "AlwaysInclude"},
                                 {SeasonalAchievementsHandling::IncludeOnlyHalloween, "IncludeOnlyHalloween"},
                             })
NLOHMANN_JSON_SERIALIZE_ENUM(RepeatableAchievementsHandling,
                             {
                                 {RepeatableAchievementsHandling::AlwaysExclude, "AlwaysExclude"},
                                 {RepeatableAchievementsHandling::ExcludeAfterFirstCompletion,
                                  "ExcludeAfterFirstCompletion"},
                                 {RepeatableAchievementsHandling::ExcludeWhenPointCapped, "ExcludeWhenPointCapped"},
                             })

void Config::Load()
{
    auto filepath = std::filesystem::path(G::APIDefs->Paths.GetAddonDirectory(ADDON_NAME)) / ConfigFilename;
    if (std::filesystem::exists(filepath))
    {
        auto json = nlohmann::json::object();
        json = nlohmann::json::parse(std::ifstream(filepath), nullptr, false);
        json.at("Exclusions").get_to(G::Exclusions->ExcludedAchievements);
        json.at("AdventureGuidePrioritized").get_to(G::Trends->AdventureGuidePrioritized);
        json.at("SeasonalAchievementsHandling").get_to(G::Exclusions->SeasonalAchievements);
        json.at("RepeatableAchievementsHandling").get_to(G::Exclusions->RepeatableAchievements);
    }
    G::Trends->Refresh();
}

void Config::Save()
{
    auto filepath = std::filesystem::path(G::APIDefs->Paths.GetAddonDirectory(ADDON_NAME)) / ConfigFilename;
    if (!std::filesystem::exists(filepath.parent_path()))
        std::filesystem::create_directories(filepath.parent_path());
    auto json = nlohmann::json::object();
    json["Exclusions"] = G::Exclusions->ExcludedAchievements;
    json["SeasonalAchievementsHandling"] = G::Exclusions->SeasonalAchievements;
    json["RepeatableAchievementsHandling"] = G::Exclusions->RepeatableAchievements;
    json["AdventureGuidePrioritized"] = G::Trends->AdventureGuidePrioritized;
    std::ofstream(filepath) << json;
}
void Config::Render()
{
    ImGui::Text("This addons relies on GW2 Efficiency for proper functioning.");
    ImGui::Text("By creating an account and registering your API key, you can make the recommendations more accurate.");
    if (ImGui::Button("Visit GW2 Efficiency"))
        ShellExecute(nullptr, nullptr, "https://gw2efficiency.com/", nullptr, nullptr, SW_SHOW);
    ImGui::Separator();
    if (ImGui::Button("Save and apply"))
    {
        Config::Save();
        Hooks::Invalidate();
    }
    ImGui::SameLine();
    if (ImGui::Button("Refresh achievements trends"))
        G::Trends->Refresh();
    ImGui::SameLine();
    if (ImGui::Button("Report an issue (GitHub)"))
        ShellExecute(nullptr, nullptr, "https://github.com/jsantorek/GW2-" ADDON_NAME "/issues", nullptr, nullptr,
                     SW_SHOW);
    if (ImGui::Checkbox("Adventure guide achievements are prioritized", &G::Trends->AdventureGuidePrioritized))
        G::Trends->Refresh();
    constexpr std::array<const char *, 3> SeasonalNames = {"Always exclude", "Always include",
                                                           "Only include Halloween"};
    if (ImGui::BeginCombo("Seasonal achievements",
                          SeasonalNames[static_cast<size_t>(G::Exclusions->SeasonalAchievements)]))
    {
        for (auto i = 0; i < SeasonalNames.size(); i++)
        {
            const bool is_selected =
                G::Exclusions->SeasonalAchievements == static_cast<SeasonalAchievementsHandling>(i);
            if (ImGui::Selectable(SeasonalNames[i], is_selected))
                G::Exclusions->SeasonalAchievements = static_cast<SeasonalAchievementsHandling>(i);
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    constexpr std::array<const char *, 3> RepeatableNames = {"Always exclude", "Include until first completion",
                                                             "Include until AP cap is reached"};
    if (ImGui::BeginCombo("Repeatable achievements",
                          RepeatableNames[static_cast<size_t>(G::Exclusions->RepeatableAchievements)]))
    {
        for (auto i = 0; i < RepeatableNames.size(); i++)
        {
            const bool is_selected =
                G::Exclusions->RepeatableAchievements == static_cast<RepeatableAchievementsHandling>(i);
            if (ImGui::Selectable(RepeatableNames[i], is_selected))
                G::Exclusions->RepeatableAchievements = static_cast<RepeatableAchievementsHandling>(i);
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    int32_t IgnoredId = 0;
    if (ImGui::InputScalar("Ignore achievement by ID", ImGuiDataType_U32, &IgnoredId, nullptr, nullptr, nullptr,
                           ImGuiInputTextFlags_EnterReturnsTrue) &&
        IgnoredId > 0)
    {
        G::Exclusions->ExcludedAchievements.insert(IgnoredId);
        Hooks::Invalidate();
        IgnoredId = 0;
    }
    for (auto id : G::Exclusions->ExcludedAchievements)
    {
        bool tmp = true;
        if (ImGui::Checkbox(std::to_string(id).c_str(), &tmp))
        {
            G::Exclusions->ExcludedAchievements.erase(id);
            Hooks::Invalidate();
            break;
        }
    }
}
