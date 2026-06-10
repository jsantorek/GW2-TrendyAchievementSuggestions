#pragma once

#include "MasteryPoint.hpp"
#include <cstdint>
#include <optional>
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
enum class MasteryPointHandling : uint32_t
{
    IncludeAny,
    IncludeOnlyCentralTyria,
    IncludeOnlyHeartOfThorns,
    IncludeOnlyPathOfFire,
    IncludeOnlyIcebroodSaga,
    IncludeOnlyEndOfDragons,
    IncludeOnlySecretsOfTheObscure,
    IncludeOnlyJanthirWilds,
    IncludeOnlyVisionsOfEternity,
};
struct ConfigurableExclusions
{
    SeasonalAchievementsHandling SeasonalAchievements = SeasonalAchievementsHandling::AlwaysExclude;
    RepeatableAchievementsHandling RepeatableAchievements = RepeatableAchievementsHandling::ExcludeWhenPointCapped;
    std::optional<MasteryPointHandling> MasteryPoints = std::nullopt;
    bool HideEmptyCategories = false;
    std::unordered_set<uint32_t> ExcludedAchievements;
    bool IsSeasonalCategoryExcluded(uint32_t);
    bool IsAchievementExcluded(uint32_t);
    bool IsMasteryPointExcluded(MasteryPoint);
};
namespace G
{
extern ConfigurableExclusions *Exclusions;
}
