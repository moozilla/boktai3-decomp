#include "global.h"
#include "gba/m4a_internal.h"
extern u16 gUnk_03005470[];

void sub_0822B4B8(void)
{
    s32 i;
    for (i = 0; i <= 31; i++) {
        m4aMPlayStop(gMPlayTable[i].info);
        gUnk_03005470[i] = 0;
    }
    m4aSoundInit();
}
