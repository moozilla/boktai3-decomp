#include "global.h"
#include "ram.h"

void Sprite_Remove(u8 *);

void RegistryF3E4_OnTaskDestroy(u8 *p)
{
    Sprite_Remove(p + 0x34);
    gRegistryF3E4Actor = 0;
}
