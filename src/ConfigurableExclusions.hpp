#pragma once

#include "MasteryPoint.hpp"
#include <cstdint>
#include <magic_enum/magic_enum.hpp>
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
enum class CategoryFilterBehaviour : uint32_t
{
    Disabled = 0,
    DisplayCategoriesWithAchievements = 1 << 0,
    DisplayDailies = 1 << 1,
    DisplayFestivals = 1 << 2,
};
template <> struct magic_enum::customize::enum_range<CategoryFilterBehaviour>
{
    static constexpr auto is_flags = false;
};
struct ConfigurableExclusions
{
    SeasonalAchievementsHandling SeasonalAchievements = SeasonalAchievementsHandling::AlwaysExclude;
    RepeatableAchievementsHandling RepeatableAchievements = RepeatableAchievementsHandling::ExcludeWhenPointCapped;
    std::optional<MasteryPointHandling> MasteryPoints = std::nullopt;
    CategoryFilterBehaviour CategoryFilter = CategoryFilterBehaviour::DisplayCategoriesWithAchievements;
    std::unordered_set<uint32_t> ExcludedAchievements;
    bool IsSeasonalCategoryExcluded(uint32_t);
    bool IsAchievementExcluded(uint32_t);
    bool IsMasteryPointExcluded(MasteryPoint);
};
namespace G
{
extern ConfigurableExclusions *Exclusions;
}
