#include "global.h"
// confidence=low: role inferred from callers and callback phases.
// confidence=low: approaches the scripted planar destination and returns to the standing update.
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Player_GetMovementRateScale(s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Sound_PlaySong(u32);
struct PlayerCueFields {
    u8 pad0[0xC0];
    u8 fC0;
    u8 padC1[0x1CE];
    u8 f28F;
    u8 pad290[0x112];
    u8 f3A2;
    u8 f3A3;
    u8 f3A4;
    u8 pad3A5[0x73];
    u8 f418;
    u8 pad419[0x184];
    u8 f59D;
};
#define H(o) (*(u16 *)(p + (o)))
#define S(o) (*(s16 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
static inline s32 scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline s32 scale6(s32 n)
{
    return n >= 0 ? n >> 6 : -((-n) >> 6);
}
static inline u32 flags(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u8 slowed(u8 *p)
{
    s32 active = H(0x538);
    s32 result = 0;
    if (active && H(0x534) == 5)
        result = 1;
    return result;
}
static inline void storeByte(u8 *p, u32 off, u32 value)
{
    ((u8 *)p)[off] = value;
}
#ifdef NONMATCHING
// fuzzy: 94.0%
// fix.py best version
void Player_AdvanceScriptDestination(u8 *p)
{
    s32 dx = (*((s16 *)(p + 0x590))) - (*((s16 *)(p + 0x30)));
    s32 dz = (*((s16 *)(p + 0x594))) - (*((s16 *)(p + 0x34)));
    s32 angle = Math_Atan2(dx, dz);
    s32 speed = 12;
    s32 t1;
    Player_GetMovementRateScale(12);
    if (p && slowed(p))
        speed = (speed * ((struct PlayerCueFields *)p)->fC0) >> 6;
    if (((dx * dx) + (dz * dz)) <= (speed * speed)) {
        *((u16 *)(p + 0x30)) = *((u16 *)(p + 0x590));
        *((u16 *)(p + 0x34)) = *((u16 *)(p + 0x594));
        if ((*((s8 *)(p + 0x59D))) >= 0) {
            ((struct PlayerCueFields *)p)->f3A4 = ((struct PlayerCueFields *)p)->f59D;
            ((struct PlayerCueFields *)p)->f59D = 255;
        } else
            ((struct PlayerCueFields *)p)->f3A4 = ((((u16)angle) + 0x70) >> 5) & 7;
        if (((struct PlayerCueFields *)p)->f3A4 > 4) {
            ((struct PlayerCueFields *)p)->f3A2 = 8 - ((struct PlayerCueFields *)p)->f3A4;
            ((struct PlayerCueFields *)p)->f3A3 = 1;
        } else {
            ((struct PlayerCueFields *)p)->f3A2 = ((struct PlayerCueFields *)p)->f3A4;
            ((struct PlayerCueFields *)p)->f3A3 = 0;
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(((struct PlayerCueFields *)p)->f418, 0),
                            64, 1);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
    } else {
        *((u16 *)(p + 0x38)) = scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
        *((u16 *)(p + 0x3C)) = scale12(gUnk_086149C4[angle & 255] * speed);
        if (((struct PlayerCueFields *)p)->f28F == 1) {
            if ((*((s16 *)(p + 0x3C))) < 0)
                *((u16 *)(p + 0x3C)) = scale6((*((s16 *)(p + 0x3C))) * 40);
            else
                *((u16 *)(p + 0x3C)) = scale6((*((s16 *)(p + 0x3C))) * 52);
        } else if (((struct PlayerCueFields *)p)->f28F == 2) {
            if ((*((s16 *)(p + 0x38))) < 0)
                *((u16 *)(p + 0x38)) = scale6((*((s16 *)(p + 0x38))) * 40);
            else
                *((u16 *)(p + 0x38)) = scale6((*((s16 *)(p + 0x38))) * 52);
        }
        ((struct PlayerCueFields *)p)->f3A4 = ((((angle + 16) & 255) >> 5) + 3) & 7;
        if (((struct PlayerCueFields *)p)->f3A4 > 4) {
            ((struct PlayerCueFields *)p)->f3A2 = 8 - ((struct PlayerCueFields *)p)->f3A4;
            ((struct PlayerCueFields *)p)->f3A3 = 1;
        } else {
            ((struct PlayerCueFields *)p)->f3A2 = ((struct PlayerCueFields *)p)->f3A4;
            ((struct PlayerCueFields *)p)->f3A3 = 0;
        }
        if ((!((*((u16 *)(p + 0xA0))) & 3)) && (!(*((u16 *)(p + 0x98))))) {
            if ((!((p && (*((u16 *)(p + 0x538)))) && (!(*((u16 *)(p + 0x534)))))) &&
                (!(p && ((u8)flags((u32 *)(p + 0x448), 4))))) {
                t1 = 0x28E;
                if (p && slowed(p)) {
                    if (!(*((u16 *)(p + 0xA0))))
                        Sound_PlaySong(0xC8);
                    else
                        Sound_PlaySong(0xC7);
                } else if ((*((u16 *)(p + 0x474))) || (!((struct PlayerCueFields *)p)->f28F)) {
                    if (!(*((u16 *)(p + 0xA0))))
                        Sound_PlaySong(0xCA);
                    else
                        Sound_PlaySong(0xC9);
                } else if (!(*((u16 *)(p + 0xA0))))
                    Sound_PlaySong(t1);
                else
                    Sound_PlaySong(0x28D);
            }
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(((struct PlayerCueFields *)p)->f418, 1),
                            64, 1);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_AdvanceScriptDestination);
#endif
