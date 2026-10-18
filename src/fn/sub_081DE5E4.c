#include "global.h"
struct MenuSpriteFlags {
    u32 flags;
    u8 pad4[0x5c];
};
struct MenuSprites {
    u8 pad0[0x20];
    u32 flags20;
    u8 pad24[0x84];
    u32 flagsA8;
    u8 padAC[0x84];
    u32 flags130;
    u8 pad134[0x84];
    u32 flags1B8;
    u8 pad1BC[0x84];
    struct MenuSpriteFlags entries[12];
};
void MenuSprites_SetFlagBits(struct MenuSprites *p)
{
    u32 one = 1;
    struct MenuSpriteFlags *entry = p->entries;
    s32 i = 11;
    do {
        entry->flags |= one;
        entry++;
    } while (--i >= 0);
    p->flags1B8 |= 1;
    p->flags130 |= 1;
    p->flagsA8 |= 1;
    p->flags20 |= 1;
}
