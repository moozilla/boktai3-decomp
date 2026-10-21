// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct CollectionRes {
    u32 w[8];
};
struct CollectionPos {
    u16 x, y, z, pad;
};
struct CollectionAnim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unk0A, scale, timer;
};
struct CollectionSprite {
    u32 flags;
    u8 unk04[3];
    u8 visible;
    u8 unk08[4];
    void *animData;
    u16 frame;
    u8 unk12[0xA];
    u16 x, y, z;
    u8 unk22[0xA];
};
struct CollectionAnimData {
    u8 data[0x1c];
};
struct CollectionMenu {
    u8 unk000[0x368];
    struct CollectionAnimData animData[27];
    struct CollectionSprite sprites[27];
};

extern const u32 gUnk_0824FEB4[30];
extern const u32 gUnk_0824FF2C[27];

void *memcpy(void *, const void *, u32);
struct CollectionRes *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
s32 Collection_IsComplete(void);
s32 Mod(s32, s32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void Anim_SetSequence(void *, void *, u16, u32, u32);
void AnimData_SelectPaletteBank(void *, u32);

#define CollectionMenu_InitGraphics sub_08210F04

static inline void StepAnim(struct CollectionAnim *a, struct CollectionSprite *s)
{
    const volatile u16 *entry = a->data + a->frame;

    s->frame = *entry >> 6;
    {
        u32 left = a->flags & 1;
        if (left != (((*entry & 0x30) >> 4) & 1))
            s->flags |= 4;
        else
            s->flags &= ~4;
    }
    {
        /* Load the flags before materializing the shared mask. */
        register u32 input asm("r0") = a->flags;
        u32 mask = 2;
        if ((u8)(mask & input) != (((*entry & 0x30) >> 4) & mask))
            s->flags |= 8;
        else
            s->flags &= ~8;
    }
    if ((u16)++a->timer >= a->rate) {
        a->timer = 0;
        if (a->flags & 4) {
            if (a->frame == 0) {
                if (a->flags & 8)
                    a->timer = a->rate;
                else
                    a->frame = a->count - 1;
            } else {
                a->frame--;
            }
        } else if ((u16)++a->frame >= a->count) {
            if ((u8)(a->flags & 8)) {
                a->timer = a->rate;
                a->frame = a->count - 1;
            } else {
                a->frame = 0;
            }
        }
        entry = a->data + a->frame;
        a->factor = *entry & 15;
        a->rate = (a->factor * a->scale) >> 6;
        if (a->rate == 0)
            a->rate = 1;
    }
}

/* Tentative: builds the collection menu widgets and the 27 entry sprites. */
void CollectionMenu_InitGraphics(u8 *m)
{
    u32 sequences[30];
    u32 palettes[27];
    struct CollectionRes *res;
    struct CollectionPos pos;
    s32 i;

    memcpy(sequences, gUnk_0824FEB4, sizeof(sequences));
    memcpy(palettes, gUnk_0824FF2C, sizeof(palettes));
    res = ScriptProc_Start(0xcb05, 0xe2ab);
    *(struct CollectionRes *)(m + 0x24) = *res;
    ResourceHeader_Relocate(m + 0x24, res);
    Widget_InitLinked(m + 0x44, m + 0x24, 0x56, 0x10, 0, 0, 0, 0);
    Widget_InitLinked(m + 0xa4, m + 0x24, 0x58, 0x10, 0, 0, 0, 0);
    Widget_InitLinked(m + 0x104, m + 0x24, 0x3e, 0x10, 0, 0, 0, 0);
    if (!Collection_IsComplete())
        *(u32 *)(m + 0x10c) |= 1;
    pos.x = 0xb8;
    pos.y = 0x68;
    pos.z = 0;
    for (i = 0; i < 4; i++) {
        Widget_InitLinked(m + 0x1c4 + i * 0x60, m + 0x24, 0, 0x10, 0, 0, 0, &pos);
        pos.x += 8;
        if (i == 1)
            pos.x += 8;
    }
    Widget_InitLinked(m + 0x164, m + 0x24, 0x59, 0x10, 0, 0, 0, 0);
    pos.x = 0x18;
    pos.y = 0x18;
    for (i = 0; i < 27; i++) {
        struct CollectionSprite *s;
        struct CollectionMenu *c = (struct CollectionMenu *)m;

        AnimData_Load(m + 0x368 + i * 0x1c, 0x593a);
        Sprite_Init(m + 0x65c + i * 0x2c, m + 0x368 + i * 0x1c, 0x11);
        c->sprites[i].x = pos.x;
        c->sprites[i].y = pos.y;
        c->sprites[i].z = 0;
        c->sprites[i].visible = 1;
        *(void **)(m + 0xb10) = ScriptProc_Start(0x922e, 0x4bbf);
        Anim_SetSequence(m + 0xb00, *(void **)(m + 0xb10), sequences[i], 0, 0);
        s = (struct CollectionSprite *)(m + i * 0x2c + 0x65c);
        StepAnim((struct CollectionAnim *)(m + 0xb00), s);
        {
            s32 field = 0x668;
            s32 off = i * 0x2c;
            AnimData_SelectPaletteBank(*(void **)(m + field + off), palettes[i] + 0x1f7);
        }
        if (Mod(i, 10) == 4) {
            pos.x = 0x18;
            pos.y += 0x28;
        } else {
            pos.x += 0x28;
        }
        if (Mod(i, 10) == 9) {
            pos.x = 0x18;
            pos.y = 0x18;
        }
    }
    CpuSet(*(void **)(m + 0xec), m + 0x344, 0x10);
    *(void **)(m + 0xec) = m + 0x344;
    {
        s32 v = 0xe010;
        *(u16 *)(m + 0xde) = v;
    }
}
