#include "global.h"
#include "scene/areas/area_effect_link.h"
#include "link/panel/link_slot_display_element.h"

struct LPanel {
    u8 filler0[0x28];
    struct LinkSlotDisplayElement elems[7];
    u8 filler2C8[0x38];
    u16 vals[8];
};
struct LPPeer {
    u8 pad0[0x1c];
    u8 mode;
};

void LinkPanel_SelectActiveAnimation(struct LPanel *p, s32 i)
{
    struct LPPeer *peer = gLinkSession;
    s32 mode = -1;
    if (peer)
        mode = peer->mode;
    if (mode <= 5) {
        p->elems[i].flags &= ~1;
        p->vals[i] = 0x2c;
    } else {
        p->elems[i].flags &= ~1;
        p->vals[i] = 0x2b;
    }
}
