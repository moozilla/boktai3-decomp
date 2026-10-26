#include "global.h"
extern u8 *gGameState;
void GameState_SetActionCount892(s32 x)
{
    s32 t;
    *(s16 *)(gGameState + 0x892) = x;
    t = *(s16 *)(gGameState + 0x892);
    if (t > 0x3E7)
        t = 0x3E7;
    else if (t < 0)
        t = 0;
    *(s16 *)(gGameState + 0x892) = t;
}
