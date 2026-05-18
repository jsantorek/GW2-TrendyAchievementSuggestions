#include "ConfigurableExclusions.hpp"
#include "MasteryPoint.hpp"

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

bool ConfigurableExclusions::IsMasteryPointExcluded(MasteryPoint point)
{
    if (!MasteryPoints)
        return false;
    switch (MasteryPoints.value())
    {
    case MasteryPointHandling::IncludeAny:
        return point == MasteryPoint::None;
    case MasteryPointHandling::IncludeOnlyCentralTyria:
        return point != MasteryPoint::CentralTyria;
    case MasteryPointHandling::IncludeOnlyHeartOfThorns:
        return point != MasteryPoint::HeartOfThorns;
    case MasteryPointHandling::IncludeOnlyPathOfFire:
        return point != MasteryPoint::PathOfFire;
    case MasteryPointHandling::IncludeOnlyIcebroodSaga:
        return point != MasteryPoint::IcebroodSaga;
    case MasteryPointHandling::IncludeOnlyEndOfDragons:
        return point != MasteryPoint::EndOfDragons;
    case MasteryPointHandling::IncludeOnlySecretsOfTheObscure:
        return point != MasteryPoint::SecretsOfTheObscure;
    case MasteryPointHandling::IncludeOnlyJanthirWilds:
        return point != MasteryPoint::JanthirWilds;
    case MasteryPointHandling::IncludeOnlyVisionsOfEternity:
        return point != MasteryPoint::VisionsOfEternity;
    }
    /* TODO */
    return true;
}
