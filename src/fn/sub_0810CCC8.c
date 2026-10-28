#include "global.h"
#include "area_effect_link.h"
struct LinkPanel {
    u8 pad0[0x13FD];
    u8 selection;
};
struct LinkTimeRecord {
    u8 pad0[0x24];
    s32 times[6];
    u8 pad3C[4];
};
struct LinkSession {
    u8 pad0[0x53];
    u8 active;
    u8 pad54[0xCC];
    struct LinkTimeRecord peers[2];
};
void LinkPanel_DrawTime(struct LinkTimeRecord *, u8 *, u32, s32);
static inline struct LinkTimeRecord *getPeer(s32 i)
{
    if (!(*(struct LinkSession **)&gLinkSession) ||
        (*(struct LinkSession **)&gLinkSession)->active != 1)
        return NULL;
    return &(*(struct LinkSession **)&gLinkSession)->peers[i];
}
static inline s32 clampIndex(s32 index, s32 low, s32 high)
{
    register s32 result = index;
    if (index > high)
        result = high;
    else if (index < low)
        result = low;
    return result;
}
void LinkPanel_DrawPeerTimes(struct LinkPanel *p)
{
    struct LinkTimeRecord *peer = getPeer(0);
    s32 index = p->selection - 6;
    index = clampIndex(index, 0, 5);
    LinkPanel_DrawTime(peer, (u8 *)2, 15, index);
    peer = getPeer(1);
    LinkPanel_DrawTime(peer, (u8 *)20, 15, index);
}
