#include "global.h"

void Nidhoggr_InitRelativeMotion(u8 *, s32, s32);
void Nidhoggr_UpdateAnchoredSway(u8 *, s32, s32);
void Nidhoggr_SetBehavior(u8 *, u32);
void Nidhoggr_ResetAllPartHealth(u8 *);
void Sound_PlaySongChecked(u32);

void Nidhoggr_UpdateSecondPhaseTransition(u8 *t)
{
    u8 *state = t + 0x724a;
    switch (*state) {
        case 0: {
            s32 *vel = (s32 *)(t + 0x72b4);
            s32 saved = *vel;
            Nidhoggr_InitRelativeMotion(t, 0x200, 0x980);
            *vel = saved;
            *state = 1;
            Sound_PlaySongChecked(0x408);
        }
        case 1: {
            s32 *pos, *vel;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0x980);
            pos = (s32 *)(t + 0x72a8);
            vel = (s32 *)(t + 0x72b4);
            *pos += *vel;
            if (*pos < -0x20000) {
                *pos = -0x20000;
                *vel = 0;
            }
            if (*(s32 *)(t + 0x72bc) > 0) {
                Nidhoggr_SetBehavior(t, 4);
                Nidhoggr_ResetAllPartHealth(t);
            }
            break;
        }
    }
}
