#include "global.h"
#include "ram.h"

// MATCHING_CONCESSION: register-pinning

struct Drawable {
    u32 a, b, flags;
};
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
s32 Player_GetControlFlags(void *, u32);
void CellObject_Init(void *, void *);
void Obj_StepPosition(void *);
void sub_0802E04C(void *);
void Collider_SetPosition(void *, void *, s32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void ActorDebris_Show(void *);
void ActorDebris_Hide(void *);
void PlayerEffect_SetPaletteSource(void *, u32, u32);
extern const u16 gUnk_0203B400[];

static inline u8 *offsetR1(u8 *p, u32 off)
{
    u32 offset = off;
    return p + offset;
}
static inline u8 *offsetR2(u8 *p, u32 off)
{
    u32 offset = off;
    return p + offset;
}
static inline void setCollider(u8 *p)
{
    u16 *ptr = (u16 *)(p + 0x236);
    register u32 value asm("r1") = *ptr;
    register u32 mask asm("r0") = 4;
    *ptr = mask | value;
}
void Player_ApplyControlPresentation(u8 *p)
{
    u32 value = H(0x474);
    if (value == 0) {
        if (Player_GetControlFlags(p, 16))
            W(0x40) = value;
        else if (!W(0x40)) {
            void *position = p + 0x284;
            CellObject_Init(position, p + 0x30);
            W(0x40) = (u32)position;
        }
    }
    Obj_StepPosition(p + 0x24);
    sub_0802E04C(p + 0x24);
    if (Player_GetControlFlags(p, 8))
        setCollider(p);
    else {
        H(0x236) &= ~4;
        Collider_SetPosition(p + 0x230, p + 0x30, 0);
    }
    if (Player_GetControlFlags(p, 1))
        ((struct Drawable *)(p + 0x8C))->flags |= 1;
    else
        ((struct Drawable *)(p + 0x8C))->flags &= ~1;
    if (Player_GetControlFlags(p, 2))
        W(0x94) |= 0x200;
    else
        W(0x94) &= ~0x200;
    if (Player_GetControlFlags(p, 64))
        *(u32 *)offsetR1(p, 0x14C) &= ~1;
    else
        *(u32 *)offsetR2(p, 0x14C) |= 1;
    if (Player_GetControlFlags(p, 4) || !*offsetR1(p, 0x740))
        ActorShadow_Disable(offsetR2(p, 0x700));
    else
        ActorShadow_Enable(offsetR1(p, 0x700));
    if (Player_GetControlFlags(p, 256) && !Player_GetControlFlags(p, 1)) {
        ActorDebris_Show((void *)W(0xC84));
        PlayerEffect_SetPaletteSource((void *)W(0xC84), H(0xC6), W(0xD4));
    } else
        ActorDebris_Hide((void *)W(0xC84));
    H(0x32) += H(0x4EC);
    *(u64 *)(p + 0xAC) = *(u64 *)(p + 0x30);
    H(0xAC) += H(0x4E6);
    H(0xAE) += H(0x4E8);
    H(0xB0) += H(0x4EA);
    if (B(0x53D)) {
        s32 jitter;
        u32 random;
        u32 mask;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        random = *(const u16 *)((gRandomIndex << 1) + (u32)gUnk_0203B400);
        mask = 15;
        jitter = (random & mask) - 7;
        H(0xAC) += jitter;
        H(0xB0) -= jitter;
        B(0x53D)--;
    }
}
