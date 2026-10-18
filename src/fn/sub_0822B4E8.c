#include "global.h"
extern u16 gUnk_03005470[];
void Sound_StopSong(u16);
void Sound_StopPlayer30Song(void)
{
    if (gUnk_03005470[0x3c / 2] != 0)
        Sound_StopSong(gUnk_03005470[0x3c / 2]);
}
