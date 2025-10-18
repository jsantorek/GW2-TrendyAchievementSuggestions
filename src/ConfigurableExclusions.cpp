#include "ConfigurableExclusions.hpp"

bool ConfigurableExclusions::IsSeasonalCategoryExcluded(uint32_t ID)
{
    switch (SeasonalAchievements)
    {
    case SeasonalAchievementsHandling::AlwaysExclude:
        return true;
    case SeasonalAchievementsHandling::AlwaysInclude:
        return false;
    case SeasonalAchievementsHandling::IncludeOnlyHalloween:
        return !(ID == 191 || ID == 192 || ID == 193);
    }
}

bool ConfigurableExclusions::IsAchievementExcluded(uint32_t ID)
{
    return ExcludedAchievements.contains(ID);
}
