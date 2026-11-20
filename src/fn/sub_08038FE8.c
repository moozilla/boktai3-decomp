#include "wireless_trade_menu.h"
#include "global.h"
void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);
void Hud_SetMode(u32);
void Video_SetRenderCallbacks(u32, void *, void *, void *);
void Script_Call(u32, u32);
void SpriteOam_QueueProjectedSprites(void);
void Sprite_RenderScaledList(void);
void sub_08219624(void);
void WirelessTradeMenu_UpdateExit(struct WirelessGameExchange *p)
{
    if (p->entered != 0) {
        p->entered = 0;
        ScreenTransition_SendCommand(3, 3, 0, 0, 0, 0xFFFF, 0);
    }
    if (p->timer == 10) {
        Hud_SetMode(7);
        Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                                 sub_08219624);
        if (p->completionScript != 0)
            Script_Call(p->completionScript, 0);
    }
    p->timer += 1;
}
