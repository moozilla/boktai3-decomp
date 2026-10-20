#include "global.h"

void SpriteOam_QueueProjectedSprites(void);
void sub_08214B44(void);
void sub_08219624(void);
void Video_SetRenderCallbacks(u32, void (*)(void), void (*)(void), void (*)(void));
u32 WirelessExit_OnDestroy(void)
{
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, sub_08214B44, sub_08219624);
    return 0;
}
