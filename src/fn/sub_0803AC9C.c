#include "wireless_game_items_and_display.h"

int WirelessGameDisplay_SetState(struct WirelessGameDisplay *p, int state)
{
    struct WirelessGameDrawing *d;
    int i;
    u32 mask;
    p->state = state;
    p->entered = 1;
    p->timer = 0;
    p->finished = 0;
    d = p->drawings;
    mask = 1;
    i = 3;
    do {
        d->flags |= mask;
        i--;
        d++;
    } while (i >= 0);
    return 0;
}
