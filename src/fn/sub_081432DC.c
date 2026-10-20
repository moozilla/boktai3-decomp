#include "global.h"

// confidence=low: approaches the saved interaction position, then holds an energy-consuming action.
#ifdef NONMATCHING
// fuzzy: 88.6%
#define B(n) (*(u8 *)(p + (n)))
#define H(n) (*(u16 *)(p + (n)))
#define S(n) (*(s16 *)(p + (n)))
extern u8 *gUnk_02000710;
extern const s16 gUnk_086149C4[];
s32 Player_GetMovementSpeed(void *);
s32 Math_Atan2(s32, s32);
void Player_UpdateFacingSide(void *);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void Player_UpdateHeldInteraction(u8 *p)
{
    s32 song;
    switch (B(0x458)) {
        case 0: {
            s32 x = S(0x498) - S(0x30);
            s32 z = S(0x49C) - S(0x34);
            s32 speed = Player_GetMovementSpeed(p);
            B(0xAC9) = 0;
            if (x * x + z * z <= speed * speed) {
                speed = 0;
                H(0x38) = x;
                H(0x3C) = z;
                B(0x3A4) = H(0x4A0);
                Player_UpdateFacingSide(p);
                Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 0), 64, 1);
                if (B(0x3A4) > 4) {
                    B(0x3A2) = speed;
                    B(0x3A3) = 1;
                } else {
                    B(0x3A2) = speed;
                    B(0x3A3) = speed;
                }
                ActorSequence_Begin(p, 8, 1);
            } else {
                s32 angle = Math_Atan2(x, z);
                H(0x38) = scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                H(0x3C) = scale(gUnk_086149C4[angle & 255] * speed);
                B(0x3A4) = (((u16)angle + 0x70) >> 5) & 7;
                Player_UpdateFacingSide(p);
                Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 1), 64, 1);
            }
            break;
        }
        case 1:
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 12), 64, 1))
                break;
            Sound_PlaySong(0xCB);
            if (!(**(u16 **)(p + 0x348) & 1))
                goto release;
            if (B(0x418) == 6)
                goto empty;
            if (H(0x428) >= H(0x42A)) {
                song = 0x12E;
                goto cue;
            }
            if (*(s32 *)(gUnk_02000710 + 0x77C) <= 0)
                goto empty;
            Sound_PlaySong(0xD8);
            B(0x4FD) = 1;
            ActorSequence_Begin(p, 8, 2);
            break;
        case 2:
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 13), 64, 1);
            if (!(**(u16 **)(p + 0x348) & 1)) {
                Sound_StopSong(0xD8);
                B(0x4FD) = 0;
                goto release;
            }
            if (H(0x428) >= H(0x42A)) {
                Sound_StopSong(0xD8);
                B(0x4FD) = 0;
                song = 0x12E;
                goto cue;
            }
            if (*(s32 *)(gUnk_02000710 + 0x77C) > 0)
                goto holding;
            {
                Sound_StopSong(0xD8);
                B(0x4FD) = 0;
                goto empty;
            }
        holding:
            *(u32 *)(p + 0x20) |= 0x40;
            break;
        empty:
            song = 0x124;
            goto cue;
        cue:
            Sound_PlaySong(song);
            ActorSequence_Begin(p, 8, 3);
            break;
        case 3:
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 14), 64, 1);
            if (!(**(u16 **)(p + 0x348) & 1)) {
            release:
                ActorSequence_Begin(p, 8, 4);
            }
            break;
        case 4:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 15), 64, 1)) {
                ActorSequence_Begin(p, 0, 0);
                Player_UpdateFacingSide(p);
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Player_UpdateHeldInteraction);
#endif
