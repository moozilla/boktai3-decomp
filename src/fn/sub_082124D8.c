// MATCHING_CONCESSION: register-pinning
#include "global.h"

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
    u8 restC[0x14];
    struct IbPos position;
    u8 rest28[0x38];
};
struct IbHalfword {
    u16 value;
};
struct IbSprite {
    u8 pad0[7];
    u8 visible;
    u8 scaleX, scaleY;
    u8 padA[6];
    u16 style;
    u8 pad12[10];
    struct IbPos position;
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
    struct IbSprite *sprite;
    void *animation;
    u16 coordinate;
    u32 zero;
    u32 loopZero;
    register struct IbPos *posPtr asm("r4");
    struct IbWidget *w;
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
    for (i = 0, loopZero = 0, posPtr = &pos, w = (void *)(p + 0x3a8); i < 18; i++) {
        Widget_InitLinked(w, p + 0x388, (u16)(0xf0 + i), 0x10, loopZero, loopZero, loopZero,
                          loopZero);
        w->position.x = posPtr->x;
        w->position.y = posPtr->y;
        w->position.z = loopZero;
        if (Mod(i, 6) == 5) {
            posPtr->x = 0x58;
            posPtr->y += 0x18;
        } else {
            posPtr->x += 0x18;
        }
        ++w;
    }

    zero = 0;
    {
        u32 *r5 = (u32 *)(p + 0x10c);
        CpuSet((void *)(*r5), p + 0x364, 0x10);
        *r5 = (u32)(p + 0x364);
        ((struct IbHalfword *)(p + 0xfe))->value = 0xe010;
    }
    {
        struct IbRes *objectRes = ScriptProc_Start(0xcb05, 0xde23);
        ResourceHeader_Relocate(p + 0xa68, objectRes);
    }
    Widget_InitLinked(p + 0xa88, p + 0xa68, zero, zero, zero, 1, zero, zero);
    *((u32 *)(p + 0xa90)) &= ~1;
    *((u8 *)(p + 0xaa2)) = 2;
    {
        u32 *r5 = (u32 *)(p + 0xad0);
        CpuSet((void *)(*r5), p + 0xaec, 0x10);
        *r5 = (u32)(p + 0xaec);
    }
    coordinate = gUnk_030042A0.x - 456;
    *((u16 *)(p + 0xaa8)) = coordinate;
    coordinate = gUnk_030042A0.y;
    *((u16 *)(p + 0xaaa)) = coordinate;
    coordinate = gUnk_030042A0.z + 336;
    *((u16 *)(p + 0xaac)) = coordinate;
    *((u8 *)(p + 0xae8)) = 3;
    *((u8 *)(p + 0xae9)) = 1;
    *((void **)(p + 0xb58)) = ActorDebris_Create(p + 0xa88, p + 0xae8, p + 0xae9);
    *((void **)(p + 0xb5c)) = PlayerAttackEffect_Create(p + 0xa88, p + 0xbe6, p + 0xae9);
    animation = p + 0xb3c;
    AnimData_Load(animation, 0x1c1b);
    animation = p + 0xb3c;
    sprite = (void *)(p + 0xb10);
    Sprite_Init(sprite, animation, 0x212);
    sprite->style = 5;
    *((u8 *)(p + 0xb17)) = 1;
    *((u16 *)(p + 0xb2c)) = 0x2e;
    *((u16 *)(p + 0xb2e)) = 0x48;
    *((u16 *)(p + 0xb30)) = zero;
    *((u8 *)(p + 0xb18)) = 0x40;
    *((u8 *)(p + 0xb19)) = 0x40;
}
