#include "global.h"

// MATCHING_CONCESSION: register-pinning
#define gRandomTableIndex gRandomIndex
#define gRandomTable      gUnk_0203B400

extern u32 gRandomTableIndex;
extern const u16 gRandomTable[];
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
static inline void setFlag(u8 *p, u32 mask)
{
    *p |= mask;
}
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
        u32 r;
        register s32 d asm("r1");
        u32 one;
        gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
        r = *(const u16 *)((gRandomTableIndex << 1) + (u32)gRandomTable);
        one = 1;
        r &= one;
        {
            register s32 negative asm("r2") = -1;
            asm("" : "+r"(negative));
            d = negative;
        }
        if (r)
            d = 1;
        p->direction = d;
        p->byte11E = one;
        p->byte11F = one;
        {
            u32 flags = p->flags119;
            u32 value = one;
            asm("" : "+r"(value) : "r"(flags));
            p->flags119 = value | flags;
        }
    }
}
