#include "global.h"
struct EntryIds {
    s32 id[5];
};
extern const struct EntryIds gUnk_0824FC6C;
struct BestiaryConfig {
    u8 pad[8];
    u32 sequence;
    u8 padC[2];
    u8 animation, flags;
    u8 pad10[8];
    s8 x, y;
    u8 rest[10];
};
extern const struct BestiaryConfig gUnk_086126E0[];
void sub_081D396C(s32, u8 *, u8 *, u32);
s32 DisplayAnimation_InitializeKeyframes(void *, u32, u32, void *);
s32 DisplayAnimation_InitializeSequence(void *, u32, u32, u32, void *);
void DisplayAnimation_SetEntry(void *, u32, u32, u32, u32, u32, u32);
void DisplayAnimation_UpdatePlayback(void *);
void DisplayAnimation_SetPriority(void *, s32);
#define BestiaryDisplay_CreateEntryAnimations sub_081D474C
void BestiaryDisplay_CreateEntryAnimations(u8 *p)
{
    struct EntryIds ids = gUnk_0824FC6C;
    u8 *animations = p + 0x28;
    const struct BestiaryConfig *config = &gUnk_086126E0[*(u32 *)(p + 0x1c)];
    s32 i;
    s32 *id;
    u8 *animation;
    if (!*(u16 *)(p + 0x26)) {
        s32 x = config->x, y = config->y;
        struct Position {
            s16 x, y, z, w;
        } *pos = (void *)(p + 0x9ec);
        x += 64;
        y += 84;
        pos->x = x;
        pos->y = y;
        pos->z = 0;
        pos->w = 0;
    }
    {
        u32 sequence = config->sequence;
        *(u16 *)(p + 0xa00) = sequence;
    }
    sub_081D396C(config->animation, p + 0x9fe, p + 0x9ff, *(u16 *)(p + 0xa00));
    i = 4;
    id = &ids.id[4];
    for (; i >= 0; i--) {
        animation = animations + i * 0x94;
        if (DisplayAnimation_InitializeKeyframes(animation, 0x7516, 16, p + 0x9ec) < 0)
            return;
        DisplayAnimation_SetEntry(p + 0x28 + i * 0x94, *id, p[0x9fe], p[0x9ff], 1, 0, 5);
        DisplayAnimation_UpdatePlayback(p + 0x28 + i * 0x94);
        DisplayAnimation_SetPriority(animation, 3);
        id--;
    }
    animation = animations + 0x2e4;
    if (DisplayAnimation_InitializeSequence(animation, 0x4147, 0xc5e2, 16, p + 0x9ec) < 0)
        return;
    *(u16 *)(*(u8 **)(animations + 0x370) + 2) -= 12;
    DisplayAnimation_SetEntry(p + 0x30c, 4, p[0x9fe], p[0x9ff], 1, 0, 5);
    DisplayAnimation_UpdatePlayback(p + 0x30c);
    DisplayAnimation_SetPriority(animation, 2);
}
