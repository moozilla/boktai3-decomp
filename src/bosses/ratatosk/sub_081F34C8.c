#include "global.h"

u8 *SceneActors_GetBySlot(u32);

u8 *Ratatosk_GetPosition(void)
{
    return SceneActors_GetBySlot(4) + 0x54;
}
