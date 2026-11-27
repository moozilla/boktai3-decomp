#include "global.h"

u8 *SceneActors_GetBySlot(u32);

s32 Ratatosk_GetSelectedPointIndex(void)
{
    return *(s16 *)(SceneActors_GetBySlot(4) + 0xE4C);
}
