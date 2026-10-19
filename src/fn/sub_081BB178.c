#include "global.h"
#ifdef NONMATCHING
// fuzzy: 94.7%
// foundry draft (worker best 94.4%); what differs is mostly register allocation and instruction order
#define ResourceSprite_GetEntry sub_081BB178
struct ResourceSpriteEntry {
    s32 values[4];
};
struct ResourceSpriteHeader {
    u32 words[8];
};
struct ResourceSprite {
    u8 pad0[0x18];
    u8 sprite[0x60];
    struct ResourceSpriteHeader resource;
    u8 pad98[8];
    u16 selection;
    s16 entryCount;
    struct ResourceSpriteEntry entries[16];
};
struct ResourceSpriteEntry *sub_081BB178(struct ResourceSprite *object, s32 index)
{
    if (!object || index < 0 || index >= object->entryCount)
        return 0;
    return &object->entries[index];
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081BB178);
#endif
