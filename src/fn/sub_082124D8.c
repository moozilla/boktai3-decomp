#include "global.h"

#ifdef NONMATCHING
// fuzzy: 82.4%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining: constant/register materialization order in the tail, widget loop base address.
struct IbRes {
    u32 w[8];
};
struct IbPos {
    u16 x, y, z, pad;
};
struct IbWidget {
    u8 b0[8];
    u32 flags;
    u8 rest[0x54];
};
struct IbCamera {
    u16 x, y, z;
};
extern struct IbCamera gUnk_030042A0;
struct IbRes *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);
s32 ForgingBook_IsCategoryComplete(void *, s32);
s32 Mod(s32, s32);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void *ActorDebris_Create(void *, void *, void *);
void *PlayerAttackEffect_Create(void *, void *, void *);
u32 ForgingBook_InitSprites(u8 *p)
{
    struct IbRes *res;
    struct IbPos pos;
    s32 i;
    u8 *w;
    u32 t1;
    res = ScriptProc_Start(0xcb05, 0xe2ab);
    *((struct IbRes *)(p + 0x24)) = *res;
    ResourceHeader_Relocate(p + 0x24, res);
    Widget_InitLinked(p + 0x64, p + 0x24, 0x49, 0x10, 0, 0, 0, 0);
    Widget_InitLinked(p + 0x124, p + 0x24, 0x3e, 0x10, 0, 0, 0, 0);
    if (!ForgingBook_IsCategoryComplete(p, *((s16 *)(p + 0xb60))))
        *((u32 *)(p + 0x12c)) |= 1;
    pos.x = 0xb8;
    pos.y = 0x68;
    pos.z = 0;
    for (i = 0; i < 4; i++) {
        Widget_InitLinked((p + 0x1e4) + (i * 0x60), p + 0x24, 0, 0x10, 0, 0, 0, &pos);
        pos.x += 8;
        if (i == 1)
            pos.x += 8;
    }

    Widget_InitLinked(p + 0x184, p + 0x24, 0x4c, 0x10, 0, 0, 0, 0);
    res = ScriptProc_Start(0xcb05, 0x530d);
    *((struct IbRes *)(p + 0x44)) = *res;
    ResourceHeader_Relocate(p + 0x44, res);
    pos.x = 0x58;
    pos.y = 0x18;
    pos.z = 0;
    Widget_InitLinked(p + 0xc4, p + 0x44, 0x4d, 0x10, 0, 0, 0, &pos);
    res = ScriptProc_Start(0xcb05, 0x5d04);
    *((struct IbRes *)(p + 0x388)) = *res;
    ResourceHeader_Relocate(p + 0x388, res);
    pos.x = 0x58;
    pos.y = 0x18;
    w = p + 0x3a8;
    for (i = 0; i < 18; i++) {
        Widget_InitLinked(w, p + 0x388, (u16)(0xf0 + i), t1 = 0x10, 0, 0, 0, 0);
        *((u16 *)(w + 0x20)) = pos.x;
        *((u16 *)(w + 0x22)) = pos.y;
        *((u16 *)(w + 0x24)) = 0;
        if (Mod(i, 6) == 5) {
            pos.x = 0x58;
            pos.y += 0x18;
        } else {
            pos.x += 0x18;
        }
        w += t1 + 80;
    }

    {
        u32 *r5 = (u32 *)(p + 0x10c);
        CpuSet((void *)(*r5), p + 0x364, 0x10);
        *r5 = (u32)(p + 0x364);
        *((u16 *)(p + 0xfe)) = 0xe010;
    }
    res = ScriptProc_Start(0xcb05, 0xde23);
    ResourceHeader_Relocate(p + 0xa68, res);
    Widget_InitLinked(p + 0xa88, p + 0xa68, 0, 0, 0, 1, 0, 0);
    *((u32 *)(p + 0xa90)) &= ~1;
    *((u8 *)(p + 0xaa2)) = 2;
    {
        u32 *r5 = (u32 *)(p + 0xad0);
        CpuSet((void *)(*r5), p + 0xaec, 0x10);
        *r5 = (u32)(p + 0xaec);
    }
    *((u16 *)(p + 0xaa8)) = gUnk_030042A0.x - 456;
    *((u16 *)(p + 0xaaa)) = gUnk_030042A0.y;
    *((u16 *)(p + 0xaac)) = gUnk_030042A0.z + 336;
    *((u8 *)(p + 0xae8)) = 3;
    *((u8 *)(p + 0xae9)) = 1;
    *((void **)(p + 0xb58)) = ActorDebris_Create(p + 0xa88, p + 0xaac, p + 0xae9);
    *((void **)(p + 0xb5c)) = PlayerAttackEffect_Create(p + 0xa88, p + 0xbe6, p + 0xae9);
    AnimData_Load(p + 0xb3c, 0x1c1b);
    Sprite_Init(p + 0xb10, p + 0xb3c, 0x212);
    *((u16 *)(p + 0xb20)) = 5;
    *((u8 *)(p + 0xb17)) = 1;
    *((u16 *)(p + 0xb2c)) = 0x2e;
    *((u16 *)(p + 0xb2e)) = 0x48;
    *((u16 *)(p + 0xb30)) = 0;
    *((u8 *)(p + 0xb18)) = 0x40;
    *((u8 *)(p + 0xb19)) = 0x40;
}

#else
INCLUDE_ASM("asm/nonmatching", ForgingBook_InitSprites);
#endif
