#include "ConfigurableExclusions.hpp"
#include "EfficiencyTrends.hpp"
#include <Config.hpp>
#include <Hooks.hpp>
#include <Nexus.h>
#include <imgui.h>

void AddonLoad(AddonAPI *aApi);
void AddonUnload();

namespace G
{
AddonAPI *APIDefs = nullptr;
ConfigurableExclusions *Exclusions = nullptr;
EfficiencyTrends *Trends = nullptr;
} // namespace G

extern "C" __declspec(dllexport) AddonDefinition *GetAddonDef();

AddonDefinition *GetAddonDef()
{
    static AddonDefinition def{
        .Signature = 0x776E7A33,
        .APIVersion = NEXUS_API_VERSION,
        .Name = "Trendy Achievement Suggestions",
        .Version = AddonVersion{ADDON_VERSION_MAJOR, ADDON_VERSION_MINOR, ADDON_VERSION_PATCH, ADDON_VERSION_REVISION},
        .Author = "Vonsh.1427",
        .Description = "",
        .Load = AddonLoad,
        .Unload = AddonUnload,
        .Flags = EAddonFlags_None,
        .Provider = EUpdateProvider_GitHub,
        .UpdateLink = "https://github.com/jsantorek/GW2-" ADDON_NAME,
    };
    return &def;
}

void AddonLoad(AddonAPI *aApi)
{
    G::APIDefs = aApi;
    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext *>(G::APIDefs->ImguiContext));
    ImGui::SetAllocatorFunctions(reinterpret_cast<void *(*)(size_t, void *)>(G::APIDefs->ImguiMalloc),
                                 reinterpret_cast<void (*)(void *, void *)>(G::APIDefs->ImguiFree));
    G::APIDefs->Renderer.Register(ERenderType_OptionsRender, Config::Render);
    G::Exclusions = new ConfigurableExclusions();
    try
    {
        G::Trends = new EfficiencyTrends();
        Config::Load();
        Hooks::Enable();
        G::APIDefs->Log(ELogLevel_INFO, ADDON_NAME, "Loaded!");
    }
    catch (const std::exception &e)
    {
        G::APIDefs->Log(ELogLevel_CRITICAL, ADDON_NAME, e.what());
    }
}

void AddonUnload()
{
    G::APIDefs->Renderer.Deregister(Config::Render);
    Hooks::Disable();
    Config::Save();
    delete G::Exclusions;
    delete G::Trends;
    G::APIDefs = nullptr;
}
