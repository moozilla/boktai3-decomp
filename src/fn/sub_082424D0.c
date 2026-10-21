// MATCHING_CONCESSION: register-pinning
#include "global.h"

#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
s32 Player_GetSecondaryActionFlags(void *, u32);
void CellObject_Init(void *, void *);
void Obj_StepPosition(void *);
void Collider_SetPosition(void *, void *, s32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);

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
void sub_082424D0(u8 *p)
{
    u32 value = H(0x474);
    if (value == 0) {
        if (Player_GetSecondaryActionFlags(p, 16))
            W(0x40) = value;
        else if (!W(0x40)) {
            void *position = p + 0x284;
            CellObject_Init(position, p + 0x30);
            W(0x40) = (u32)position;
        }
    }
    Obj_StepPosition(p + 0x24);
    if (Player_GetSecondaryActionFlags(p, 8))
        setCollider(p);
    else {
        H(0x236) &= ~4;
        Collider_SetPosition(p + 0x230, p + 0x30, 0);
    }
    if (Player_GetSecondaryActionFlags(p, 1))
        W(0x94) |= 1;
    else
        W(0x94) &= ~1;
    if (Player_GetSecondaryActionFlags(p, 2))
        W(0x94) |= 0x200;
    else
        W(0x94) &= ~0x200;
    if (Player_GetSecondaryActionFlags(p, 64))
        *(u32 *)offsetR1(p, 0x14C) &= ~1;
    else
        *(u32 *)offsetR2(p, 0x14C) |= 1;
    if (Player_GetSecondaryActionFlags(p, 4))
        ActorShadow_Disable(offsetR2(p, 0x700));
    else
        ActorShadow_Enable(offsetR2(p, 0x700));
    H(0x32) += H(0x4EC);
    *(u64 *)(p + 0xAC) = *(u64 *)(p + 0x30);
    H(0xAE) += H(0x4E8);
}
