#include "global.h"
extern struct Manager *gEffectQuad;
extern u8 *gUnk_020004B4;
void SpriteDrawable_SelectTileFrame(void *, u32, s32);

struct Pair {
    u32 a, b;
};
struct Entry {
    u8 state;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u32 unk4;
    u8 drawable[0x28];
    struct Pair pos;
};
struct Manager {
    u8 pad[0x20];
    struct Entry entries[4];
    u8 pad2[0x100 - 0x20 - 4 * 56];
    u32 resource;
};

static inline s32 GetCurrent(void)
{
    if (gUnk_020004B4 == NULL)
        return -1;
    return *(s16 *)(gUnk_020004B4 + 0x18);
}

void sub_08232FD0(s32 index, struct Pair *pos)
{
    struct Entry *e;
    if (gEffectQuad == NULL)
        return;
    if (GetCurrent() == index)
        return;
    e = &gEffectQuad->entries[index];
    e->pos = *pos;
    SpriteDrawable_SelectTileFrame(e->drawable, gEffectQuad->resource, 53);
    {
        u8 st = 2;
        e->state = st;
        e->unk4 = 0;
        e->unk2 = 1;
    }
}
