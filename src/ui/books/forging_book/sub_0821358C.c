#include "global.h"

struct CollectionRes {
    u32 w[8];
};
struct CollectionPos {
    u16 x, y, z, pad;
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
struct CollectionBookMenu {
    u8 unk000[0x500];
    struct CollectionSprite sprites[10];
};

extern const u32 gUnk_08614054[];
extern const u32 gUnk_0861407C[];
extern const u32 gUnk_086140A4[];

struct CollectionRes *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
s32 CollectionBook_IsComplete(void);
s32 Mod(s32, s32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void CpuSet(const void *, void *, u32);

void sub_0821358C(u8 *m)
{
    struct CollectionRes *res;
    struct CollectionPos pos;
    s32 i;
    u16 k;
    u32 k2;

    res = ScriptProc_Start(0xcb05, 0xe2ab);
    *(struct CollectionRes *)(m + 0x24) = *res;
    ResourceHeader_Relocate(m + 0x24, res);
    Widget_InitLinked(m + 0x64, m + 0x24, 0x57, 0x10, 0, 0, 0, 0);
    Widget_InitLinked(m + 0xc4, m + 0x24, 0x58, 0x10, 0, 0, 0, 0);
    Widget_InitLinked(m + 0x124, m + 0x24, 0x3e, 0x10, 0, 0, 0, 0);
    if (!CollectionBook_IsComplete())
        *(u32 *)(m + 0x12c) |= 1;
    pos.x = 0xb8;
    pos.y = 0x68;
    pos.z = 0;
    for (i = 0; i < 4; i++) {
        Widget_InitLinked(m + 0x1e4 + i * 0x60, m + 0x24, 0, 0x10, 0, 0, 0, &pos);
        pos.x += 8;
        if (i == 1)
            pos.x += 8;
    }
    Widget_InitLinked(m + 0x184, m + 0x24, 0x59, 0x10, 0, 0, 0, 0);
    res = ScriptProc_Start(0xcb05, 0x530d);
    *(struct CollectionRes *)(m + 0x44) = *res;
    ResourceHeader_Relocate(m + 0x44, res);
    Widget_InitLinked(m + 0x364, m + 0x44, 0x50, 0x10, 0, 0, 0, 0);
    pos.x = 0x28;
    pos.y = 0x28;
    for (i = 0; i < 10; i++) {
        const u32 *frame;
        struct CollectionSprite *s;
        struct CollectionBookMenu *c = (struct CollectionBookMenu *)m;

        AnimData_Load(m + 0x3e8 + i * 0x1c, 0xca1f);
        Sprite_Init(m + 0x500 + i * 0x2c, m + 0x3e8 + i * 0x1c, 0x11);
        frame = &gUnk_08614054[i];
        s = (struct CollectionSprite *)(i * 0x2c + (u32)m);
        s = (struct CollectionSprite *)((u8 *)s + 0x500);
        s->frame = *frame;
        c->sprites[i].x = pos.x;
        c->sprites[i].y = pos.y;
        c->sprites[i].z = 0;
        c->sprites[i].visible = 1;
        {
            s32 field = 0x50c;
            s32 off = i * 0x2c;
            AnimData_SelectPaletteBank(*(void **)(m + field + off), gUnk_086140A4[i] + 0x17d);
        }
        if (Mod(i, 10) == 4) {
            pos.x = 0x28;
            pos.y += 0x28;
        } else {
            pos.x += 0x28;
        }
        if (Mod(i, 10) == 9) {
            pos.x = 0x28;
            pos.y = 0x28;
        }
        k = 0x6e0;
        k2 = 2;
        *(u16 *)(m + k + i * k2) = gUnk_0861407C[i];
    }
    CpuSet(*(void **)(m + 0x10c), m + 0x3c4, 0x10);
    *(void **)(m + 0x10c) = m + 0x3c4;
    {
        s32 v = 0xe010;
        *(u16 *)(m + 0xfe) = v;
    }
}
