#include "global.h"
s32 sub_08165A30(void *);
void Sound_PlaySong(s32);
void sub_0822DA94(s32);
void sub_08173A7C(void *);
void sub_08174078(u8 *p)
{
    s32 r = sub_08165A30(p);
    if (r == 0) {
        Sound_PlaySong(0xde);
        sub_08173A7C(p);
    } else if (r == 1) {
        Sound_PlaySong(0xdd);
        sub_0822DA94(0);
        p[0x4388] = 0;
        p[0x4389] = 0;
        p[0x438a] = 0;
        sub_08173A7C(p);
    }
}
