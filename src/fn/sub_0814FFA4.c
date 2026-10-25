// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: MoveTo callback approaches destination and decelerates its movement effect.
extern const s16 gUnk_086149C4[];
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
s32 Math_Atan2(s32, s32);
void Script_Call(u32, u32);
void Player_ConstrainTetherTarget(void *, s32, s32, u32);
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
    if (B(0x3A4) > 4) {
        putB(p, 0x3A2, 8 - B(0x3A4));
        B(0x3A3) = 1;
    } else {
        B(0x3A2) = B(0x3A4);
        B(0x3A3) = 0;
    }
}
#ifdef NONMATCHING
// fuzzy: 82.4%
// fix.py best version
inline static void putB_UpdateMoveDestination(u8 *p, u32 o, u8 v)
{
    p[o] = v;
}

inline static u32 Arg1(u32 k_, u8 *p)
{
    return p + k_;
}

void PlayerCue_UpdateMoveDestination(u8 *p)
{
    s32 dx = (*((s16 *)(p + 0x590))) - (*((s16 *)(p + 0x30)));
    s32 dz = (*((s16 *)(p + 0x594))) - (*((s16 *)(p + 0x34)));
    register s32 distance asm("r8") = (dx * dx) + (dz * dz);
    register s32 x asm("sl") = 0;
    register s32 z asm("sb") = 0;
    s32 moving;
    s32 anim;
    s32 angle;
    typeof(angle) c1;
    if (!distance) {
        anim = ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 16);
        if ((*((s8 *)(p + 0x59D))) >= 0) {
            *((u8 *)Arg1(0x3A4, p)) = *((u8 *)(p + 0x59D));
            *((u8 *)(p + 0x59D)) = 255;
            face(p);
        }
        if (*((u8 *)(p + 0x59A))) {
            if (*((u8 *)(p + 0x59B))) {
                moving = 1;
                angle = (((*((u8 *)(p + 0x3A4))) + 1) & 7) << 5;
                x = gUnk_086149C4[(angle + 64) & 255];
                z = gUnk_086149C4[angle & 255];
                *((u8 *)(p + 0x59B)) = ((*((u8 *)(p + 0x59B))) * 7) >> 3;
            } else {
                moving = 0;
                if (*((u32 *)(p + 0x5A4))) {
                    u32 script = *((u32 *)(p + 0x5A4));
                    *((u32 *)(p + 0x5A4)) = 0;
                    Script_Call(script, 0);
                }
                *((u8 *)(p + 0x59A)) = 0;
            }
        } else {
            moving = 0;
            if (*((u32 *)(p + 0x5A4))) {
                u32 script = *((u32 *)(p + 0x5A4));
                *((u32 *)(p + 0x5A4)) = 0;
                Script_Call(script, 0);
            }
        }
    } else {
        anim = ActorVariant_GetTableValue(*((u8 *)(p + 0x418)), 17);
        moving = 1;
        angle = Math_Atan2(dx, dz);
        c1 = angle;
        putB_UpdateMoveDestination(p, 0x3A4, ((((u16)angle) + 0x70) >> 5) & 7);
        face(p);
        angle = (((*((u8 *)(p + 0x3A4))) + 1) & 7) << 5;
        x = gUnk_086149C4[(angle + 64) & 255];
        z = gUnk_086149C4[c1 & 255];
        if ((*((u16 *)(p + 0xA0))) & 3) {
            if (distance <= 64) {
                *((u16 *)(p + 0x30)) = *((u16 *)(p + 0x590));
                *((u16 *)(p + 0x34)) = *((u16 *)(p + 0x594));
            } else {
                *((u16 *)(p + 0x38)) = -scale12(x * 8);
                *((u16 *)(p + 0x3C)) = -scale12(z * 8);
                if ((*((u8 *)(p + 0x28F))) == 1) {
                    if ((*((s16 *)(p + 0x3C))) < 0)
                        *((u16 *)(p + 0x3C)) = scale6((*((s16 *)(p + 0x3C))) * 40);
                    else
                        *((u16 *)(p + 0x3C)) = scale6((*((s16 *)(p + 0x3C))) * 52);
                } else if ((*((u8 *)(p + 0x28F))) == 2) {
                    if ((*((s16 *)(p + 0x38))) < 0)
                        *((u16 *)(p + 0x38)) = scale6((*((s16 *)(p + 0x38))) * 40);
                    else
                        *((u16 *)(p + 0x38)) = scale6((*((s16 *)(p + 0x38))) * 52);
                }
            }
            if ((*((u8 *)(p + 0x59A))) && ((*((u8 *)(p + 0x59B))) <= 149))
                *((u8 *)(p + 0x59B)) = ((*((u8 *)(p + 0x59B))) * 15) >> 4;
        }
    }
    Player_SetAnimation(p, anim, 64, 1);
    if (moving)
        Player_ConstrainTetherTarget(p, x, z, *((u8 *)(p + 0x59B)));
}

#else
INCLUDE_ASM("asm/nonmatching", PlayerCue_UpdateMoveDestination);
#endif
