#include "global.h"

void Sound_PlaySong(u32);

void sub_080A0874(u8 *p)
{
    u8 *q = *(u8 **)(p + 4) + 0x48;
    if (*(u16 *)(q + 8) == 5 && *(u16 *)(q + 0xe) == 0)
        Sound_PlaySong(0xf6);
}
