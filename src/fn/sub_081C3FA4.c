#include "global.h"

extern u32 gUnk_02000268;
void Sprite_Remove(u8 *);

void RegistryF3E4_OnTaskDestroy(u8 *p)
{
    Sprite_Remove(p + 0x34);
    gUnk_02000268 = 0;
}
