#include "global.h"

void sub_08195014(u8 *, s32, s32);
void sub_08195078(u8 *, s32, s32);
void sub_08195A4C(u8 *, u32);
void sub_081959C0(u8 *);
void Sound_PlaySongChecked(u32);

void sub_08195E6C(u8 *t)
{
    u8 *state = t + 0x724a;
    switch (*state) {
    case 0: {
        s32 *vel = (s32 *)(t + 0x72b4);
        s32 saved = *vel;
        sub_08195014(t, 0x200, 0x980);
        *vel = saved;
        *state = 1;
        Sound_PlaySongChecked(0x408);
    }
    case 1: {
        s32 *pos, *vel;
        sub_08195078(t, 0x200, 0x980);
        pos = (s32 *)(t + 0x72a8);
        vel = (s32 *)(t + 0x72b4);
        *pos += *vel;
        if (*pos < -0x20000) {
            *pos = -0x20000;
            *vel = 0;
        }
        if (*(s32 *)(t + 0x72bc) > 0) {
            sub_08195A4C(t, 4);
            sub_081959C0(t);
        }
        break;
    }
    }
}
