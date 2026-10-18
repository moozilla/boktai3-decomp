#include "global.h"

extern u8 gUnk_08605CE4[];

void Ghoul_SetSpecialHandlers(u8 *p)
{
    *(u8 **)(p + 0x28c) = gUnk_08605CE4;
}
