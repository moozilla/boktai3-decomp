#include "global.h"
#define gSessionMenu gUnk_020004B4
#include "area_effect_link.h"
#include "engine.h"
#include "ram.h"
void Serial_StopCommunication(void);
void Video_SetRenderCallbacks(s32, void (*)(void), void (*)(void), void (*)(void));
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
s32 TextWindow_Clear(s32);
s32 SessionMenu_Destroy(struct AreaEffectLinkState *p)
{
    Serial_StopCommunication();
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    gLinkMenuActive = 0;
    gEngineFlags &= ~0x800;
    if (p->menuTextWindow >= 0 && TextWindow_Clear(p->menuTextWindow) == 0)
        p->menuTextWindow = -1;
    gClockSynced = 0;
    gUnk_020004B4 = 0;
    return 0;
}
