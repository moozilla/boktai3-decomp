#include "global.h"
#include "ram.h"

void SlotRegistry_RemoveSprite(s32);

s32 NativeEffect_DestroyManager(void)
{
    s32 i;
    for (i = 0; i <= 0xf; i++)
        SlotRegistry_RemoveSprite(i);
    gSceneCallbackPool = 0;
    return 0;
}
