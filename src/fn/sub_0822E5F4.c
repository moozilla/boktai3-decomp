#include "global.h"
extern u8 *gGameState;
extern u32 gUnk_08E60F48[];
u32 *GameState_GetUnlockDescriptor(u32 i)
{
    return &gUnk_08E60F48[*(s16 *)(gGameState + i * 2 + 0x4c0)];
}
