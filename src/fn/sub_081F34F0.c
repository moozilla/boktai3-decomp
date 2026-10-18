#include "global.h"

u8 *SceneActors_GetBySlot(u32);

s32 Ratatosk_GetSelectedTargetIndex(void)
{
    return *(s16 *)(SceneActors_GetBySlot(4) + 0xE4E);
}
