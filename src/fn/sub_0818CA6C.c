#include "global.h"
#include "engine_globals.h"
#include "ram.h"

struct ArmorPart {
    u8 pad0[9];
    s8 stretchY;
    u8 padA[0x119 - 0xA];
    u8 flags119;
    u8 pad11A[4];
    u8 byte11E;
    u8 byte11F;
    u8 pad120[0x1DA - 0x120];
    s8 direction;
    u8 pad1DB[4];
    u8 stretchDeltaY;
};
void Armor_RetractPartStretch(struct ArmorPart *p)
{
    s32 stretch = p->stretchY;
    if (stretch > 64) {
        stretch += (s8)p->stretchDeltaY;
        if (stretch < 64)
            stretch = 64;
    }
    p->stretchY = stretch;
    if (stretch == 64) {
        u32 random;
        u8 one;
        u32 offset;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        offset = gRandomIndex * 2;
        random = *(const u16 *)(offset + (u32)gRandomTable);
        one = 1;
        random &= one;
        p->direction = random ? 1 : -1;
        p->byte11E = one;
        p->byte11F = one;
        p->flags119 = one | p->flags119;
    }
}
