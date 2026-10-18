#include "global.h"
struct WirelessSessionConfig {
    u8 maxPlayers, interval;
    u16 flags;
    u8 mode, unknown5;
    u16 serialNumber;
    const void *gameName, *userName;
    u8 unknown10, enabled;
    u16 timeout, retryTimeout;
};
u32 Wireless_InitSessionConfig(struct WirelessSessionConfig *config, s32 active, u32 serialNumber,
                               u16 flags, u32 mode, const void *gameName, const void *userName)
{
    config->maxPlayers = 4;
    config->interval = 40;
    config->mode = mode;
    config->serialNumber = serialNumber;
    config->gameName = gameName;
    config->enabled = 1;
    config->timeout = 240;
    config->retryTimeout = 480;
    if (active)
        config->flags = flags;
    else
        config->flags = 0;
    config->userName = userName;
}
