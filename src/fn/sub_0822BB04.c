#include "global.h"
extern u8 *gGameState;
u8 *Save_GetContinuationData(void)
{
    return gGameState + 0x778;
}
