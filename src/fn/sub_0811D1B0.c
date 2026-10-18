#include "global.h"
extern u8 *gUnk_02000710;
u8 *GameState_GetBikeName(void)
{
    return gUnk_02000710 + 0x790;
}
