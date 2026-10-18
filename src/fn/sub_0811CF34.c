#include "global.h"
u8 SceneSlot_RegisterObject(u32);
void CharacterModel_SetPrimaryAppearance(u8 *p, u32 x)
{
    *(u8 *)(p + 0x250) = SceneSlot_RegisterObject(x);
}
