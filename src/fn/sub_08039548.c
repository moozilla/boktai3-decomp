#include "global.h"

void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void Video_SetRenderCallbacks(u32, void (*)(void), void (*)(void), void (*)(void));
u32 WirelessExit_OnDestroy(void)
{
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    return 0;
}
