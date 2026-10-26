#include "global.h"
extern u8 *gGameState;
s16 GameState_GetActionCount894(void)
{
    s32 t = *(s16 *)(gGameState + 0x894);
    if (t > 0x3E7)
        t = 0x3E7;
    else if (t < 0)
        t = 0;
    *(s16 *)(gGameState + 0x894) = t;
    return *(s16 *)(gGameState + 0x894);
}
