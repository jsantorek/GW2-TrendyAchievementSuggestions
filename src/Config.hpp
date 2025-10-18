#pragma once

struct Config
{
    static constexpr auto ConfigFilename = "Config.json";

    static void Load();
    static void Save();
    static void Render();
};