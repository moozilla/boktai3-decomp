#include "global.h"
#include "minigames/result_game/result_menu_marker.h"
typedef struct ResultMenuMarkerGraphic Graphic;
typedef struct {
    u16 state, time;
} State;
void Anim_LoadKeyframe(void *, void *, int, int);
void ResultMenu_EnqueueMarker(u8 *p)
{
    State *s = (State *)(p + 0x78c + p[0x4a] * 4);
    Graphic *g = (Graphic *)(p + 0x384 + p[0x4a] * 0x60);
    Anim_LoadKeyframe(g, p + 0x104, p[0xbf] + 0xc2, 0);
    g->flags &= ~0x201;
    s->state = 1;
    s->time = 0;
    if (++p[0x4a] > 4)
        p[0x4a] = 0;
}
