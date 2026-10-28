#include "global.h"
#include "area_effect_link.h"

struct LPElem {
    u32 flags;
    u8 filler4[0x5C];
};
struct LPanel {
    u8 filler0[0x28];
    struct LPElem elems[7];
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
