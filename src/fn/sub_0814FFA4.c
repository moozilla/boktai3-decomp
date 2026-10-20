#define Player_SetAnimation sub_0813AE18
#define ActorVariant_GetTableValue sub_081419D4
// confidence=low: tentative cue role inferred from named callers and callback behavior.
#define PlayerCue_UpdateMoveDestination sub_0814FFA4
#define Script_Call sub_0821AD08
#define Math_Atan2 sub_082215E4
// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: MoveTo callback approaches destination and decelerates its movement effect.
extern const s16 gUnk_086149C4[];
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
s32 Math_Atan2(s32, s32);
void Script_Call(u32, u32);
void sub_08143B54(void *, s32, s32, u32);
#define B(o) (*(u8 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define S(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline s32 scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline s32 scale6(s32 n)
{
    return n >= 0 ? n >> 6 : -((-n) >> 6);
}
static inline void putB(u8 *p, u32 o, u32 v)
{
    p[o] = v;
}
static inline void face(u8 *p)
{
    u32 dir = B(0x3A4);
    if (dir > 4) {
        putB(p, 0x3A2, 8 - dir);
        B(0x3A3) = 1;
    } else {
        B(0x3A2) = dir;
        B(0x3A3) = 0;
    }
}
#ifdef NONMATCHING
// fuzzy: 80.0%
void PlayerCue_UpdateMoveDestination(u8 *p)
{
    s32 dx = S(0x590) - S(0x30), dz = S(0x594) - S(0x34);
    register s32 distance asm("r8") = dx * dx + dz * dz;
    register s32 x asm("sl") = 0;
    register s32 z asm("sb") = 0;
    s32 moving, anim, angle;
    if (!distance) {
        anim = ActorVariant_GetTableValue(B(0x418), 16);
        if (*(s8 *)(p + 0x59D) >= 0) {
            B(0x3A4) = B(0x59D);
            B(0x59D) = 255;
            face(p);
        }
        if (B(0x59A)) {
            if (B(0x59B)) {
                moving = 1;
                angle = ((B(0x3A4) + 1) & 7) << 5;
                x = gUnk_086149C4[(angle + 64) & 255];
                z = gUnk_086149C4[angle & 255];
                B(0x59B) = B(0x59B) * 7 >> 3;
            } else {
                moving = 0;
                if (W(0x5A4)) {
                    u32 script = W(0x5A4);
                    W(0x5A4) = 0;
                    Script_Call(script, 0);
                }
                B(0x59A) = 0;
            }
        } else {
            moving = 0;
            if (W(0x5A4)) {
                u32 script = W(0x5A4);
                W(0x5A4) = 0;
                Script_Call(script, 0);
            }
        }
    } else {
        anim = ActorVariant_GetTableValue(B(0x418), 17);
        moving = 1;
        angle = Math_Atan2(dx, dz);
        putB(p, 0x3A4, (((u16)angle + 0x70) >> 5) & 7);
        face(p);
        angle = ((B(0x3A4) + 1) & 7) << 5;
        x = gUnk_086149C4[(angle + 64) & 255];
        z = gUnk_086149C4[angle & 255];
        if (H(0xA0) & 3) {
            if (distance <= 64) {
                H(0x30) = H(0x590);
                H(0x34) = H(0x594);
            } else {
                H(0x38) = -scale12(x * 8);
                H(0x3C) = -scale12(z * 8);
                if (B(0x28F) == 1) {
                    if (S(0x3C) < 0)
                        H(0x3C) = scale6(S(0x3C) * 40);
                    else
                        H(0x3C) = scale6(S(0x3C) * 52);
                } else if (B(0x28F) == 2) {
                    if (S(0x38) < 0)
                        H(0x38) = scale6(S(0x38) * 40);
                    else
                        H(0x38) = scale6(S(0x38) * 52);
                }
            }
            if (B(0x59A) && B(0x59B) <= 149)
                B(0x59B) = B(0x59B) * 15 >> 4;
        }
    }
    Player_SetAnimation(p, anim, 64, 1);
    if (moving)
        sub_08143B54(p, x, z, B(0x59B));
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0814FFA4);
#endif
