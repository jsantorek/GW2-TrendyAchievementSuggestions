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
NLOHMANN_JSON_SERIALIZE_ENUM(MasteryPointHandling,
                             {
                                 {MasteryPointHandling::IncludeAny, "IncludeAny"},
                                 {MasteryPointHandling::IncludeOnlyCentralTyria, "IncludeOnlyCentralTyria"},
                                 {MasteryPointHandling::IncludeOnlyHeartOfThorns, "IncludeOnlyHeartOfThorns"},
                                 {MasteryPointHandling::IncludeOnlyPathOfFire, "IncludeOnlyPathOfFire"},
                                 {MasteryPointHandling::IncludeOnlyIcebroodSaga, "IncludeOnlyIcebroodSaga"},
                                 {MasteryPointHandling::IncludeOnlyEndOfDragons, "IncludeOnlyEndOfDragons"},
                                 {MasteryPointHandling::IncludeOnlySecretsOfTheObscure,
                                  "IncludeOnlySecretsOfTheObscure"},
                                 {MasteryPointHandling::IncludeOnlyJanthirWilds, "IncludeOnlyJanthirWilds"},
                                 {MasteryPointHandling::IncludeOnlyVisionsOfEternity, "IncludeOnlyVisionsOfEternity"},
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
        if (json.contains("MasteryPoints"))
            G::Exclusions->MasteryPoints = json.at("MasteryPoints").get<MasteryPointHandling>();
        else
            G::Exclusions->MasteryPoints = std::nullopt;
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
    if (G::Exclusions->MasteryPoints.has_value())
    {
        json["MasteryPoints"] = G::Exclusions->MasteryPoints.value();
    }
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
    static bool OnlyMasteries = G::Exclusions->MasteryPoints.has_value();
    if (ImGui::Checkbox("Display only achievements with Mastery Point", &OnlyMasteries))
    {
        if (OnlyMasteries)
            G::Exclusions->MasteryPoints = MasteryPointHandling::IncludeAny;
        else
            G::Exclusions->MasteryPoints = std::nullopt;
        Hooks::Invalidate();
    }
    if (OnlyMasteries)
    {
        constexpr auto MasteryNames = std::array{
            "Any",
            "Only Central Tyria",
            "Only Heart of Thorns",
            "Only Path of Fire",
            "Only Icebrood Saga",
            "Only End of Dragons",
            "Only Secrets of the Obscure",
            "Only Janthir Wilds",
            "Only Visions of Eternity",
        };
        if (ImGui::BeginCombo("Masteries are included",
                              MasteryNames[static_cast<size_t>(G::Exclusions->MasteryPoints.value())]))
        {
            for (auto i = 0; i < MasteryNames.size(); i++)
            {
                const bool is_selected = G::Exclusions->MasteryPoints == static_cast<MasteryPointHandling>(i);
                if (ImGui::Selectable(MasteryNames[i], is_selected))
                {
                    G::Exclusions->MasteryPoints = static_cast<MasteryPointHandling>(i);
                    Hooks::Invalidate();
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    constexpr auto SeasonalNames = std::array{"Always exclude", "Always include", "Only include Halloween"};
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
    constexpr auto RepeatableNames =
        std::array{"Always exclude", "Include until first completion", "Include until AP cap is reached"};
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
