#include "global.h"
#include "ram.h"
extern u8 *gGameState;
s32 Script_GetValue(void);
void Region_SetCurrentFromScript(void)
{
    *(s32 *)(gGameState + 0x5a0) = Script_GetValue();
    gRegionOverride = 1;
}
