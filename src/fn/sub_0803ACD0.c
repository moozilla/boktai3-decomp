#include "global.h"
#include "engine.h"
#include "wireless_game_items_and_display.h"
void WirelessGameDisplay_UpdateStatusIcons(struct WirelessGameDisplay *p)
{
    int value = WirelessGame_GetRemotePlayerHealthPercent();
    int category = 0;
    if (value <= 60) {
        category = 2;
        if (value > 25)
            category = 1;
    }
    Anim_LoadKeyframe(&p->extra[0], &p->extraResource, (u16)(category + 405), 0);
    {
        int value = WirelessGame_GetRemoteOpponentHealthPercent();
        int category = 0;
        if (value <= 60) {
            category = 2;
            if (value > 25)
                category = 1;
        }
        Anim_LoadKeyframe(&p->extra[1], &p->extraResource, (u16)(category | 408), 0);
    }
}
