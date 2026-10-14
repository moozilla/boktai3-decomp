#include "global.h"

void sub_08195014(u8 *, s32, s32);
void sub_08195078(u8 *, s32, s32);
void sub_08195A4C(u8 *, u32);
void sub_081959C0(u8 *);

void sub_08196674(u8 *t)
{
    u8 *state = t + 0x724a;
    switch (*state) {
        case 0:
            sub_08195014(t, 0x200, 0x980);
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
            sub_08195078(t, 0x200, 0x980);
            if (*(s16 *)(t + 0x5404) <= 0) {
                sub_08195A4C(t, 4);
                sub_081959C0(t);
            }
            break;
        }
    }
}
