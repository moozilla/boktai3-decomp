#include "global.h"
#include "solar_bank_panel.h"
#include "input.h"
#include "game_state.h"

s32 Anim_LoadKeyframe(void *, void *, u16, u32);
void Anim_Step(void *, void *);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void SolarBank_BeginClosing(struct SolarBankPanel *);
void SolarBank_BeginSelection(struct SolarBankPanel *);
void SolarBank_RefreshAmountDigits(struct SolarBankPanel *);

// MATCHING_CONCESSION: register-pinning
#define SolarBank_UpdateTransferInput sub_081BB904
void SolarBank_UpdateTransferInput(struct SolarBankPanel *panel)
{
    s32 v;
    u16 t;
    u8 repeatMask;

    if (gPadState[1] & 1) {
        {
            *((u32 *)&gGameState->unk77C) = panel->balance;
            *((u16 *)&gGameState->unk7B0) = panel->transferAmount;
        }
        Sound_PlaySong(0xdd);
        SolarBank_BeginClosing(panel);
        goto step;
    }
    if (gPadState[1] & 2) {
        Sound_PlaySong(0xde);
        SolarBank_BeginSelection(panel);
        goto step;
    }
    if (gPadState[0] & 0x20) {
        u32 *bal = (u32 *)&panel->balance;
        u32 *xfer;
        u16 *hold;
        u16 *timer;
        u16 h;
        if (*bal > 9998)
            goto reset;
        xfer = (u32 *)&panel->transferAmount;
        if (*xfer == 0)
            goto reset;
        if (panel->holdFrames == 0)
            panel->repeatSpeed = 0;
        hold = &panel->holdFrames;
        panel->transferDirection = 0;
        timer = &panel->animationTimer;
        *timer = 16;
        h = *hold;
        if (*hold > 0x7f) {
            s32 amount, room;
            u32 b;
            Sound_PlaySong(0x37d);
            amount = 10;
            if (*xfer < amount)
                amount = *xfer;
            room = 10;
            b = *bal;
            if (b + 10 > 9999)
                room = 9999 - b;
            if (amount < room) {
                *bal = b + amount;
                *xfer -= amount;
            } else {
                *bal = b + room;
                *xfer -= room;
            }
            SolarBank_RefreshAmountDigits(panel);
            panel->repeatSpeed = 2;
            panel->animationTimer = 4;
        } else if (*hold > 0x3f) {
            repeatMask = 3;
            repeatMask &= h;
            if (!repeatMask)
                Sound_PlaySong(0x37d);
            (*bal)++;
            (*xfer)--;
            SolarBank_RefreshAmountDigits(panel);
            panel->repeatSpeed = 1;
            *timer = 4;
            (*hold)++;
        } else {
            *hold = h + 1;
            repeatMask = 7;
            repeatMask &= h;
            if (!repeatMask) {
                Sound_PlaySong(0x37d);
                (*bal)++;
                (*xfer)--;
                SolarBank_RefreshAmountDigits(panel);
            }
        }
    } else if ((t = gPadState[0] & 0x10) != 0) {
        register u32 balOffset asm("r1") = 0x6F8;
        u32 *bal;
        u32 *xfer;
        u16 *hold;
        u16 *timer;
        u16 h;
        asm("" : : "r"(balOffset));
        bal = (u32 *)((u8 *)panel + balOffset);
        if (*bal == 0)
            goto reset;
        {
            register u32 offset asm("r3") = 0x6F4;
            asm("" : : "r"(offset));
            xfer = (u32 *)((u8 *)panel + offset);
        }
        if (*xfer > 9998)
            goto reset;
        if (panel->holdFrames == 0)
            panel->repeatSpeed = 0;
        hold = &panel->holdFrames;
        panel->transferDirection = 1;
        timer = &panel->animationTimer;
        *timer = 16;
        h = *hold;
        if (*hold > 0x7f) {
            s32 amount, room;
            u32 b;
            Sound_PlaySong(0x37d);
            amount = 10;
            if (*bal < amount)
                amount = *bal;
            room = 10;
            b = *xfer;
            if (b + 10 > 9999)
                room = 9999 - b;
            if (amount < room) {
                *xfer = b + amount;
                *bal -= amount;
            } else {
                *xfer = b + room;
                *bal -= room;
            }
            SolarBank_RefreshAmountDigits(panel);
            panel->repeatSpeed = 2;
            panel->animationTimer = 4;
        } else if (*hold > 0x3f) {
            repeatMask = 3;
            repeatMask &= h;
            if (!repeatMask)
                Sound_PlaySong(0x37d);
            (*xfer)++;
            (*bal)--;
            SolarBank_RefreshAmountDigits(panel);
            panel->repeatSpeed = 1;
            *timer = 4;
            (*hold)++;
        } else {
            *hold = h + 1;
            repeatMask = 7;
            repeatMask &= h;
            if (!repeatMask) {
                Sound_PlaySong(0x37d);
                (*xfer)++;
                (*bal)--;
                SolarBank_RefreshAmountDigits(panel);
            }
        }
    } else {
        goto zero;
    }
    goto blink;
reset:
    panel->holdFrames = 0;
    goto blink;
zero:
    panel->holdFrames = t;
blink:
    if (panel->animationTimer) {
        if (panel->repeatSpeed == 2)
            panel->blinkPhase += 4;
        else if (panel->repeatSpeed == 1)
            panel->blinkPhase += 2;
        else
            panel->blinkPhase += 1;
        panel->animationTimer--;
    } else {
        panel->blinkPhase = 0;
    }
    if (panel->blinkPhase == 0) {
        AnimData_SelectPaletteBank(&panel->indicators[0], 0x1AE);
        AnimData_SelectPaletteBank(&panel->indicators[1], 0x1B1);
        Anim_LoadKeyframe(panel->unk1D8, panel->labelData, 0x12, 1);
    } else {
        switch ((panel->blinkPhase & 0x1f) >> 3) {
            case 1:
                v = 2;
                break;
            case 0:
            case 2:
                v = 1;
                break;
            default:
                v = 0;
                break;
        }
        AnimData_SelectPaletteBank(&panel->indicators[0], 0x1AE + v);
        AnimData_SelectPaletteBank(&panel->indicators[1], 0x1B1 + v);
        Anim_LoadKeyframe(panel->unk1D8, panel->labelData, panel->transferDirection * 3 + 0x13 + v,
                          1);
    }
step:
    Anim_Step(&panel->label, panel->labelData);
}
