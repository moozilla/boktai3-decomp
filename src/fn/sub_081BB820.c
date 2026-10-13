#include "global.h"
void Sound_PlaySong(u32);
void Anim_Step(u8 *, u8 *);
void sub_081BB5CC(u8 *);
void sub_081BB820(u8 *p)
{
    u16 *c = (u16 *)(p + 0x70c);
    if (*c == 0x20)
        Sound_PlaySong(0x2be);
    Anim_Step(p + 0x58, p + 0x18);
    (*c)++;
    if (*c > 0x68)
        sub_081BB5CC(p);
}
