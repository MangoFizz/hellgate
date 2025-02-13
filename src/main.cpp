// SPDX-License-Identifier: GPL-3.0-only

#include <balltze/logger.hpp>
#include <balltze/plugin.hpp>

Balltze::Logger logger("Hellgate");

BALLTZE_PLUGIN_API Balltze::PluginMetadata plugin_metadata() {
    return {
        "Hellgate",
        "MangoFizz",
        { 1, 0, 0 },
        { 1, 0, 0 },
        {},
        true,
    };
}

BALLTZE_PLUGIN_API bool plugin_load() noexcept {
    logger.info("Loaded!");
    return true;
}

WINAPI BOOL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
}
