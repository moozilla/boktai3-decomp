// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct PhaseEntryView {
    u32 flags;
    u8 pad4[2];
    u8 phase;
    u8 pad7;
    s8 opacity;
    u8 pad9[0x13];
    s16 x, y, z, pad22;
    u8 pad24[8];
};
struct PhasePoint {
    s16 x, y, z, pad;
};
struct PhaseObjectView {
    u8 pad0[0x60];
    u8 *anchor;
    s16 deltaX, pad66, deltaZ, pad6A;
    s16 growthX, pad6E, growthZ;
    u8 pad72[0x1B];
    u8 flags8D;
    u8 pad8E[4];
    u8 enabled, mode;
    u8 pad94[0x14];
    struct PhaseEntryView entries[4];
    u8 pad158[0x468 - 0x158];
    struct PhasePoint saved[4];
    struct PhasePoint target;
    u8 pad490[0xB68 - 0x490];
    u32 mainFlags;
    u8 padB6C[2];
    u8 phase;
    u8 padB6F[0xB84 - 0xB6F];
    struct PhasePoint origin;
    u8 padB8C[0xC22 - 0xB8C];
    u16 colliderFlagsA;
    u8 padC24[0xC76 - 0xC24];
    u16 colliderFlagsB;
    u8 padC78[0xCC8 - 0xC78];
    void *action;
    u8 padCCC[0xCE0 - 0xCCC];
    void *callback;
    u8 padCE4[0x10];
    s32 delay;
    u8 padCF8[0xD80 - 0xCF8];
    u32 flagsD80;
};
s32 Div(s32, s32);

void ChildGroup_UpdateAlternatingCallbacks(void);
void PhaseObject_UpdateEntryScatter(void);
// confidence=low: interpolates the four entries toward the shared target before enabling them.
void PhaseObject_ConvergeEntries(struct PhaseObjectView *p)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        struct PhaseObjectView *entry;
        s32 x = Div((p->target.x - p->saved[i].x) * p->delay, 15);
        entry = (void *)((u8 *)p + i * 0x2C);
        entry->entries[0].x = p->target.x + x;
        entry->entries[0].z = p->target.z + Div((p->target.z - p->saved[i].z) * p->delay, 15);
        entry->entries[0].opacity += 3;
        if (entry->entries[0].opacity > 63) {
            entry->entries[0].opacity = 64;
        }
    }
    if (!(p->flagsD80 & 1) && --p->delay == 0) {
        u8 *enabled = &p->enabled;
        u8 *mode = &p->mode;
        u8 *flags8D = &p->flags8D;
        s16 *targetX = &p->target.x;
        s16 *targetZ = &p->target.z;
        s8 *opacity = &p->entries[0].opacity;
        u32 offset = 0;
        u32 opacityValue = 64;
        u32 *flags = &p->entries[0].flags;
        u32 bit = 1;
        for (i = 3; i >= 0; i--) {
            *(s16 *)(opacity + 0x14) = *targetX;
            *(s16 *)(opacity + 0x18) = *targetZ;
            *opacity = opacityValue;
            *(u32 *)((u8 *)flags + offset) |= bit;
            opacity += 0x2C;
            offset += 0x2C;
        }
        p->mainFlags &= ~1;
        p->colliderFlagsA &= 0xFFFB;
        p->colliderFlagsB &= 0xFFFB;
        {
            u32 one = 1;
            u32 five = 5;
            DEBUG_NOP();
            *enabled = one;
            *mode = five;
            *flags8D = one | *flags8D;
        }
        p->action = ChildGroup_UpdateAlternatingCallbacks;
        p->callback = PhaseObject_UpdateEntryScatter;
    }
}
