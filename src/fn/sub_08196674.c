#include "global.h"

void Nidhoggr_InitRelativeMotion(u8 *, s32, s32);
void Nidhoggr_UpdateAnchoredSway(u8 *, s32, s32);
void Nidhoggr_SetBehavior(u8 *, u32);
void Nidhoggr_ResetAllPartHealth(u8 *);

void Nidhoggr_UpdateRetreat(u8 *t)
{
    u8 *state = t + 0x724a;
    switch (*state) {
        case 0:
            Nidhoggr_InitRelativeMotion(t, 0x200, 0x980);
            *(s32 *)(t + 0x72b4) = -0x600;
            (*state)++;
        case 1: {
            s32 *pos = (s32 *)(t + 0x72a8);
            s32 *vel = (s32 *)(t + 0x72b4);
            *pos += *vel;
            if (*pos < -0x20000) {
                *pos = -0x20000;
                *vel = 0;
            }
            *vel += 6;
            if (*vel > -0x30)
                *vel = -0x30;
            Nidhoggr_UpdateAnchoredSway(t, 0x200, 0x980);
            if (*(s16 *)(t + 0x5404) <= 0) {
                Nidhoggr_SetBehavior(t, 4);
                Nidhoggr_ResetAllPartHealth(t);
            }
            break;
        }
    }
}
