#include "global.h"
extern u16 gUnk_03005470[];
void m4aSongNumStart(u16);
void Sound_RestartPlayer30Song(void)
{
    if (gUnk_03005470[0x3c / 2] != 0)
        m4aSongNumStart(gUnk_03005470[0x3c / 2]);
}
