#include "global.h"
#include "ram.h"

// confidence=low: energy-gated burst sequence with a held middle animation.
#define B(n) (*(u8 *)(p + (n)))
#define H(n) (*(u16 *)(p + (n)))
struct Position {
    s16 x, y, z, pad;
};
s32 Player_ConsumeStateEntry(void *);
s32 Player_HasRequiredEnergy(void *, s32);
void ActorSequence_Begin(void *, s32, s32);
s32 ActorVariant_GetTableValue(u32, u32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void Player_UpdateFacingSide(void *);
void Sound_PlaySong(u32);
void ActorBurst_Create(void *, void *, void *, u32, u32);
static inline void ResetBurstSize(u8 *p)
{
    u16 *dst = (u16 *)(p + 0xAD6);
    u32 value;
    value = 0x16E;
    *dst = value;
}
static inline void grow(u8 *p)
{
    ResetBurstSize(p);
    if ((H(0xAD8) += 3) > 32)
        H(0xAD8) = 32;
}
void Player_UpdateHeldEnergyBurst(u8 *p)
{
    if (Player_ConsumeStateEntry(p)) {
        B(0xAC9) = 0;
        if (Player_HasRequiredEnergy(p, 100) && !gPlayerSolarActionPending)
            B(0x4EE) = 1;
        else
            B(0x4EE) = 0;
        ActorSequence_Begin(p, 9, 0);
    }
    switch (B(0x458)) {
        case 0:
            if ((u8)(B(0x3A4) - 2) <= 4) {
                B(0x3A4) = 4;
                B(0x3A2) = 4;
                B(0x3A3) = 0;
                *(struct Position *)(p + 0xA80) = *(struct Position *)(p + 0x30);
                H(0xA80) -= 20;
                H(0xA82) += 330;
                H(0xA84) += 60;
            } else {
                B(0x3A4) = 0;
                B(0x3A2) = 0;
                B(0x3A3) = 0;
                *(struct Position *)(p + 0xA80) = *(struct Position *)(p + 0x30);
                H(0xA80) += 20;
                H(0xA82) += 330;
                H(0xA84) -= 20;
            }
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 0), 64, 1);
            if (B(0x3A4) == 4)
                B(0x3A2) = 1;
            else
                B(0x3A2) = 0;
            if (B(0x4EE)) {
                Sound_PlaySong(0x128);
                ResetBurstSize(p);
                H(0xAD8) = 2;
                ActorBurst_Create(p, p + 0x30, p + 0xA80, 0, H(0x4A2));
            }
            ActorSequence_Begin(p, 9, 1);
            break;
        case 1:
            if (B(0x4EE))
                grow(p);
            H(0x6FC) = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 8), 64, 1)) {
                Sound_PlaySong(0xCB);
                if (B(0x4EE) && !H(0x4A2))
                    gPlayerSolarActionPending = 1;
                ActorSequence_Begin(p, 9, 2);
            }
            break;
        case 2:
            if (B(0x4EE))
                grow(p);
            else if (!H(0x45A))
                Sound_PlaySong(0x192);
            H(0x6FC) = 30;
            Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 10), 64, 1);
            if (++H(0x45A) > 23)
                ActorSequence_Begin(p, 9, 3);
            break;
        case 3:
            H(0x6FC) = 15;
            if (Player_SetAnimation(p, ActorVariant_GetTableValue(B(0x418), 11), 64, 1)) {
                Player_UpdateFacingSide(p);
                ActorSequence_Begin(p, 0, 0);
            }
            break;
    }
}
