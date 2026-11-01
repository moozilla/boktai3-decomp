#include "global.h"

// confidence=low: approaches the saved interaction position, then holds an energy-consuming action.
#define B(n) (*(u8 *)(p + (n)))
#define H(n) (*(u16 *)(p + (n)))
#define S(n) (*(s16 *)(p + (n)))
extern u8 *gGameState;
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
static inline void SetFacing(u8 *p, u32 facing)
{
    u32 value = facing;
    u8 *dst;
    dst = p + 0x3A4;
    *dst = value;
}
static inline u32 MaskFlags(u32 *p, u32 mask)
{
    return *p | mask;
}
static inline void ClearHolding(u8 *p)
{
    u32 offset = 0x4FD;
    u8 *state;
    register u32 zero asm("r1");
    state = p + offset;
    zero = 0;
    *state = zero;
}
static inline u32 Quantize(s32 angle)
{
    register s32 value asm("r0") = (u16)angle;
    asm("" : "+r"(value));
    value = (value + 0x70) >> 5;
    return value & 7;
}
static inline u32 ReadVariant(u8 *p)
{
    u32 offset = 0x418;
    {
        u8 *field = p + offset;
        return *field;
    }
}
static inline u32 ReadHeldInput(u8 *p)
{
    u32 offset = 0x348;
    {
        u16 **field = (u16 **)(p + offset);
        return **field & 1;
    }
}
static inline s32 ReadHeldEnergy(void)
{
    u8 *global = gGameState;
    u32 offset;
    offset = 0x77C;
    return *(s32 *)(global + offset);
}
// MATCHING_CONCESSION: register-pinning
void Player_UpdateHeldInteraction(u8 *arg)
{
    u8 *p = arg;
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
                {
                    u32 facing = H(0x4A0);
                    u8 *direction = p + 0x3A4;
                    *direction = facing;
                    Player_UpdateFacingSide(p);
                    Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 0), 64, 1);
                    if (*direction > 4) {
                        B(0x3A2) = speed;
                        B(0x3A3) = 1;
                    } else {
                        B(0x3A2) = speed;
                        B(0x3A3) = speed;
                    }
                    ActorSequence_Begin(p, 8, 1);
                }
            } else {
                s32 angle = Math_Atan2(x, z);
                H(0x38) = scale(gUnk_086149C4[(angle + 64) & 255] * speed);
                H(0x3C) = scale(gUnk_086149C4[angle & 255] * speed);
                SetFacing(p, Quantize(angle));
                Player_UpdateFacingSide(p);
                Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 1), 64, 1);
            }
            break;
        }
        case 1: {
            u8 *variant = p + 0x418;
            if (!Player_SetAnimation(p, ActorVariant_GetTableValue(*variant, 12), 64, 1))
                break;
            Sound_PlaySong(0xCB);
            if (!(**(u16 **)(p + 0x348) & 1))
                goto release;
            if (*variant == 6)
                goto empty;
            if (H(0x428) >= H(0x42A)) {
                song = 0x12E;
                goto cue;
            }
            if (*(s32 *)(gGameState + 0x77C) <= 0)
                goto empty;
            Sound_PlaySong(0xD8);
            B(0x4FD) = 1;
            ActorSequence_Begin(p, 8, 2);
            break;
        }
        case 2:
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 13), 64, 1);
            if (!(**(u16 **)(p + 0x348) & 1)) {
                Sound_StopSong(0xD8);
                B(0x4FD) = 0;
                goto release;
            }
            if (H(0x428) >= H(0x42A)) {
                Sound_StopSong(0xD8);
                ClearHolding(p);
                song = 0x12E;
                goto cue;
            }
            if (ReadHeldEnergy() > 0)
                goto holding;
            {
                Sound_StopSong(0xD8);
                ClearHolding(p);
                goto empty;
            }
        empty:
            song = 0x124;
            goto cue;
        cue:
            Sound_PlaySong(song);
            ActorSequence_Begin(p, 8, 3);
            break;
        holding:
            *(u32 *)(p + 0x20) = MaskFlags((u32 *)(p + 0x20), 0x40);
            break;
        case 3:
            Player_SetAnimation(p, ActorVariant_GetTableValue(ReadVariant(p), 14), 64, 1);
            if (!(ReadHeldInput(p))) {
            release:
                ActorSequence_Begin(p, 8, 4);
            }
            break;
        case 4:
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(ReadVariant(p), 15), 64, 1)) {
                ActorSequence_Begin(p, 0, 0);
                Player_UpdateFacingSide(p);
            }
            break;
    }
}
