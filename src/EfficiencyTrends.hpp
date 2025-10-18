#pragma once
#include <unordered_map>

class EfficiencyTrends
{
  public:
    void Refresh();
    uint32_t GetAchievementPopularity(uint32_t id);
    bool AdventureGuidePrioritized = false;

  private:
    void PrioritizeAdventureGuide();
    std::unordered_map<int, int> Raw;
};
namespace G
{
extern EfficiencyTrends *Trends;
}
