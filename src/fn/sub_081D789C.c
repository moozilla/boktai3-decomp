#define Sprite_SetOffsets Effect_SetOrigin
#include "global.h"
struct SpriteGroupEntry {
    u8 sprite[0x28];
    u8 active;
    u8 unknown29[7];
};
struct SpriteGroup {
    u8 unknown0[0x40];
    u32 state;
    void *resource;
    struct SpriteGroupEntry entries[4];
};
void *AnimData_FindEntry(s32);
void sub_08217DE0(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void Effect_SetPalette(void *, u32);
void SpriteGroup_InitFourSprites(struct SpriteGroup *p, u32 animation)
{
    u8 *active;
    struct SpriteGroupEntry *entry;
    s32 i;
    s32 offset;
    p->state = 0;
    p->resource = AnimData_FindEntry(0x1C1E);
    offset = -4;
    active = &p->entries[0].active;
    entry = p->entries;
    i = 3;
    do {
        sub_08217DE0(entry, p->resource, 1);
        Effect_SetOrigin(entry, offset, offset);
        SpriteDrawable_SelectTileFrame(entry, p->resource, animation);
        Effect_SetPalette(entry, 1);
        *active = 0;
        active += 0x30;
        ++entry;
        --i;
    } while (i >= 0);
}
