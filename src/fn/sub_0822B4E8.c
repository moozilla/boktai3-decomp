#include "global.h"
extern u16 gUnk_03005470[];
void Sound_StopSong(u16);
void sub_0822B4E8(void)
{
    if (gUnk_03005470[0x3c / 2] != 0)
        Sound_StopSong(gUnk_03005470[0x3c / 2]);
}
