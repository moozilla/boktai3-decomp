#include "link/wireless/wireless_game_items_and_display.h"

int WirelessGameDisplay_SetState(struct WirelessGameDisplay *p, int state)
{
    struct WirelessGameDrawing *d;
    int i;
    p->state = state;
    p->entered = 1;
    p->timer = 0;
    p->finished = 0;
    d = p->drawings;
    for (i = 0; i < 4; i++, d++)
        d->flags |= 1;
    return 0;
}
