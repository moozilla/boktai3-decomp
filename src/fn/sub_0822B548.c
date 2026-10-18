#include "global.h"
extern u16 gUnk_03005470[];
void m4aSongNumStop(u16);
void Sound_SuspendPlayer30Song(void)
{
    if (gUnk_03005470[0x3c / 2] != 0)
        m4aSongNumStop(gUnk_03005470[0x3c / 2]);
}
