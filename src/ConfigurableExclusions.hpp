#pragma once

#include <cstdint>
#include <unordered_set>

enum class SeasonalAchievementsHandling : uint32_t
{
    AlwaysExclude,
    AlwaysInclude,
    IncludeOnlyHalloween
};
enum class RepeatableAchievementsHandling : uint32_t
{
    AlwaysExclude,
    ExcludeAfterFirstCompletion,
    ExcludeWhenPointCapped
};
struct ConfigurableExclusions
{
    SeasonalAchievementsHandling SeasonalAchievements = SeasonalAchievementsHandling::AlwaysExclude;
    RepeatableAchievementsHandling RepeatableAchievements = RepeatableAchievementsHandling::ExcludeWhenPointCapped;
    std::unordered_set<uint32_t> ExcludedAchievements;
    bool IsSeasonalCategoryExcluded(uint32_t ID);
    bool IsAchievementExcluded(uint32_t ID);
};
namespace G
{
extern ConfigurableExclusions *Exclusions;
}
