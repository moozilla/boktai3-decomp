#include "global.h"
#define ResourceSprite_CopyEntries sub_081BB060
struct ResourceSpriteEntry { s32 values[4]; };
struct ResourceSpriteHeader { u32 words[8]; };
struct ResourceSprite {
    u8 pad0[0x18];
    u8 sprite[0x60];
    struct ResourceSpriteHeader resource;
    u8 pad98[8];
    u16 selection;
    s16 entryCount;
    struct ResourceSpriteEntry entries[16];
};
void sub_081BB060(struct ResourceSprite *track, u32 count, struct ResourceSpriteEntry *keys)
{
    s32 i;
    struct ResourceSpriteEntry *dest;
    if (count <= 16) {
        track->entryCount = count;
        i = 0;
        for (; i < track->entryCount; i++) {
            track->entries[i] = keys[i];
        }
    }
}
