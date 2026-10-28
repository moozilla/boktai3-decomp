#include "global.h"
#include "scene_hud_display.h"
#include "game_state.h"
#include "player.h"

void StatusPanel_UpdateGaugePosition(struct SceneHudState *);
void sub_0816A0D0(struct SceneHudState *);
void PauseMenu_RefreshPriceDigits(struct SceneHudState *);
s32 StatusPanel_GetDirectionAxis(u8);
s32 MessageWindow_ShowMessage(s32);
void AnimData_SelectPaletteBank(void *, int);

/* SceneHudState viewed as one widget array from 0x1420 (indices 75 and 76 are valueWidgets). */
struct SceneHudWidgetTable {
    u8 unknown0[0x1420];
    struct SceneInputWidget widgets[0x80];
};
#define WIDGET(p, n) (((struct SceneHudWidgetTable *)(p))->widgets[n])

void PauseMenu_UpdateQuantityInput(struct SceneHudState *p)
{
    u8 *timer;
    s32 bank, frame, i;

    if ((gPadState[0] & 0x20) &&
        gGameState->menuRewardBalance + (p->valueRate + 1) * p->selectedQuantity <= 9999 &&
        p->actionOwner->currentStatB + p->valueRate < p->actionOwner->maximumStatB) {
        p->valueRate++;
        Sound_PlaySong(0x37D);
        StatusPanel_UpdateGaugePosition(p);
        sub_0816A0D0(p);
        PauseMenu_RefreshPriceDigits(p);
        p->quantityArrowFrame = 0x13;
        timer = &p->quantityArrowTimer;
        bank = StatusPanel_GetDirectionAxis(*timer);
        frame = p->quantityArrowFrame + bank;
        if (++*timer >= 16)
            *timer = 0;
    } else if ((gPadState[0] & 0x10) && p->valueRate) {
        p->valueRate--;
        Sound_PlaySong(0x37D);
        StatusPanel_UpdateGaugePosition(p);
        sub_0816A0D0(p);
        PauseMenu_RefreshPriceDigits(p);
        p->quantityArrowFrame = 0x16;
        timer = &p->quantityArrowTimer;
        bank = StatusPanel_GetDirectionAxis(*timer);
        frame = p->quantityArrowFrame + bank;
        if (p->balanceLimitTimer)
            p->balanceLimitTimer = 1;
        if (++*timer >= 16)
            *timer = 0;
    } else if (p->quantityArrowTimer) {
        bank = StatusPanel_GetDirectionAxis(p->quantityArrowTimer);
        frame = p->quantityArrowFrame + bank;
        if (++p->quantityArrowTimer >= 16)
            p->quantityArrowTimer = 0;
    } else {
        bank = 0;
        frame = 0x12;
    }
    if ((gPadState[0] & 0x20) &&
        gGameState->menuRewardBalance + (p->valueRate + 1) * p->selectedQuantity > 9999)
        p->balanceLimitTimer = 60;
    if (p->balanceLimitTimer) {
        if (--p->balanceLimitTimer == 0) {
            PauseMenu_RefreshPriceDigits(p);
            MessageWindow_ShowMessage(4);
            p->balanceLimitShown = 0;
        } else {
            *(u32 *)((u8 *)p + 0x4008) |= 1;
            for (i = 0; i <= 3; i++) {
                WIDGET(p, i + 113).flags |= 1;
                WIDGET(p, i + 118).flags |= 1;
            }
            if (!p->balanceLimitShown) {
                MessageWindow_ShowMessage(6);
                p->balanceLimitShown = 1;
            }
        }
    }
    Anim_LoadKeyframe((u8 *)p + 0x3D00, p->unknownC4, (u16)frame, 1);
    AnimData_SelectPaletteBank((u8 *)p + 0xC54, bank + 0x1B4);
}
