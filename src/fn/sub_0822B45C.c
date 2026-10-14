#include "global.h"
extern u16 gUnk_03005470[];
void m4aSongNumStartOrContinue(u16);
void m4aSongNumStartOrChange(u16);
/* Restart the remembered songs of slots 10, 12, 29 and 30. */
void sub_0822B45C(void)
{
    s32 i;
    for (i = 0; i <= 31; i++) {
        if (gUnk_03005470[i] == 0)
            continue;
        if (i == 10)
            m4aSongNumStartOrContinue(gUnk_03005470[10]);
        else if (i == 12 || i == 29 || i == 30)
            m4aSongNumStartOrChange(gUnk_03005470[i]);
    }
}
