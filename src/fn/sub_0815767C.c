#include "global.h"

#ifdef NONMATCHING
// fuzzy: 88.2%

struct Drawable {
    u32 a, b, flags;
};
#define H(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
s32 Player_GetControlFlags(void *, u32);
void sub_0821BC68(void *, void *);
void sub_08225B74(void *);
void sub_0802E04C(void *);
void Collider_SetPosition(void *, void *, s32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void ActorDebris_Show(void *);
void ActorDebris_Hide(void *);
void PlayerEffect_SetPaletteSource(void *, u32, u32);
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];

void Player_ApplyControlPresentation(u8 *p)
{
    u32 value = H(0x474);
    if (value == 0) {
        if (Player_GetControlFlags(p, 16))
            W(0x40) = value;
        else if (!W(0x40)) {
            void *position = p + 0x284;
            sub_0821BC68(position, p + 0x30);
            W(0x40) = (u32)position;
        }
    }
    sub_08225B74(p + 0x24);
    sub_0802E04C(p + 0x24);
    if (Player_GetControlFlags(p, 8))
        H(0x236) = 4 | H(0x236);
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
        W(0x14C) &= ~1;
    else
        W(0x14C) |= 1;
    if (Player_GetControlFlags(p, 4) || !B(0x740))
        ActorShadow_Disable(p + 0x700);
    else
        ActorShadow_Enable(p + 0x700);
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
        gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
        jitter = (*(const u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400) & 15) - 7;
        H(0xAC) += jitter;
        H(0xB0) -= jitter;
        B(0x53D)--;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_ApplyControlPresentation);
#endif
