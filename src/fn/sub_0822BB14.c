#include "global.h"
extern u8 *gGameState;
u8 *Save_GetContinuationDataEnd(void)
{
    return gGameState + 0x858;
}
