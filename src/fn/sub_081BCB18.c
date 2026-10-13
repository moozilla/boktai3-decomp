#include "global.h"
void Sound_PlaySong(u32);
void Anim_Step(u8 *, u8 *);
void sub_081BC76C(u8 *);
void sub_081BCB18(u8 *p)
{
    u16 *c;
    Anim_Step(p + 0x58, p + 0x18);
    c = (u16 *)(p + 0x904);
    if (*c == 0x20)
        Sound_PlaySong(0x2c0);
    (*c)++;
    if (*c > 0x68)
        sub_081BC76C(p);
}
