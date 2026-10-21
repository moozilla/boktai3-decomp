#include "global.h"
#define ResourceSprite_GetEntry Registry3F74_FindFrameOffset
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
struct ResourceSpriteEntry *Registry3F74_FindFrameOffset(struct ResourceSprite *object, s32 index)
{
    struct ResourceSprite *p = object;
    if (!p)
        return 0;
    if (index < 0 || index >= object->entryCount)
        return 0;
    return &p->entries[index];
}
