#include "global.h"
extern u8 *gGameState;
s32 GameState_GetResultTimer(void)
{
    s32 t = *(s32 *)(gGameState + 0x88c);
    if (t > 0xEA60)
        t = 0xEA60;
    else if (t < 0)
        t = 0;
    *(s32 *)(gGameState + 0x88c) = t;
    return *(s32 *)(gGameState + 0x88c);
}
