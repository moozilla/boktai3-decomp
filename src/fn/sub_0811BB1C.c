#include "global.h"
extern u8 *gGameState;
s16 GameState_GetResultIndex(void)
{
    s32 t = *(s16 *)(gGameState + 0x890);
    if (t > 0xb)
        t = 0xb;
    else if (t < 0)
        t = 0;
    *(s16 *)(gGameState + 0x890) = t;
    return *(s16 *)(gGameState + 0x890);
}
