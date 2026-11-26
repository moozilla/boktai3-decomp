#include "link/wireless/wireless_game_items_and_display.h"

u32 WirelessGameDisplay_RequestCompletion(struct WirelessGameDisplay *display)
{
    display->finished = 1;
    return 0;
}
