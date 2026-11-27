#include "global.h"
extern u8 *gGameState;
s32 Solar_GetLevel(void)
{
    return *(s16 *)(gGameState + 0x876);
}
