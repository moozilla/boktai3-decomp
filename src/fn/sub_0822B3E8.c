#include "global.h"
extern u16 gUnk_03005470[];
void m4aSongNumStop(u16);
void Sound_StopAllTrackedSongs(void)
{
    u16 *p = gUnk_03005470;
    s32 i = 0x1f;
    do {
        if (*p != 0) {
            m4aSongNumStop(*p);
            *p = 0;
        }
        p++;
        i--;
    } while (i >= 0);
}
