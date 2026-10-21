#include "global.h"

#ifdef NONMATCHING
// fuzzy: 87.9%
// remaining: flags119 OR keeps the constant in r4 (adds r0,r4,#0) in ROM
#define gRandomTableIndex gUnk_03005308
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
        s32 d;
        u32 one = 1;
        gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
        r = gRandomTable[gRandomTableIndex] & one;
        d = -1;
        if (r)
            d = 1;
        p->direction = d;
        p->byte11E = one;
        p->byte11F = one;
        p->flags119 |= one;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Armor_RetractPartStretch);
#endif
