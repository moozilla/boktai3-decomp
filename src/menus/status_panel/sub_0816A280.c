#include "engine/shared/global.h"
#include "menus/scene_hud/scene_hud_display.h"
#include "system/game_state/game_state.h"
#include "engine/shared/player.h"

void StatusPanel_UpdateGaugePosition(struct SceneHudState *);
void sub_0816A0D0(struct SceneHudState *);
void PauseMenu_RefreshPriceDigits(struct SceneHudState *);
s32 StatusPanel_GetDirectionAxis(u8);
s32 MessageWindow_ShowMessage(s32);
void AnimData_SelectPaletteBank(void *, int);

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
            p->accountRateIcon.flags |= 1;
            for (i = 0; i <= 3; i++) {
                p->accountValueDigits[i].flags |= 1;
                p->accountTotalDigits[i].flags |= 1;
            }
            if (!p->balanceLimitShown) {
                MessageWindow_ShowMessage(6);
                p->balanceLimitShown = 1;
            }
        }
    }
    Anim_LoadKeyframe(&p->accountCursor, &p->accountResource, (u16)frame, 1);
    AnimData_SelectPaletteBank((u8 *)p + 0xC54, bank + 0x1B4);
}
