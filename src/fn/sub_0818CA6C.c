
#ifdef NONMATCHING
// fuzzy: 80.3%
// foundry draft (worker best 80.3%); what differs is mostly register allocation and instruction order
#define Armor_RetractPartStretch sub_0818CA6C
#define gRandomTableIndex        gUnk_03005308
#define gRandomTable             gUnk_0203B400
#include "global.h"

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
void sub_0818CA6C(struct ArmorPart *p)
{
    s32 stretch = p->stretchY;
    if (stretch > 64) {
        stretch += (s8)p->stretchDeltaY;
        if (stretch < 64)
            stretch = 64;
    }
    p->stretchY = stretch;
    if (stretch == 64) {
        s32 direction;
        u32 random;
        gRandomTableIndex = (gRandomTableIndex + 1) & 1023;
        random = *(const u16 *)((gRandomTableIndex << 1) + (u32)gRandomTable);
        {
            s32 one = 1;
            s32 negative;
            random &= one;
            negative = -1;
            direction = negative;
            if (random)
                direction = 1;
            p->direction = direction;
            p->byte11E = one;
            p->byte11F = one;
            setFlag(&p->flags119, one);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818CA6C);
#endif
