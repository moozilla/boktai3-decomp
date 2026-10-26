#include "global.h"
u8 *Save_GetContinuationData(void);
extern u8 *gGameState;
s32 Save_GetMainStateSize(void)
{
    return Save_GetContinuationData() - gGameState;
}
