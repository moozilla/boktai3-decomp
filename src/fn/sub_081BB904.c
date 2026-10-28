#include "global.h"
#include "solar_bank_panel.h"
#include "input.h"

struct Keys {
    u16 held;
    u16 pressed;
};
extern u8 *gGameState;

s32 Anim_LoadKeyframe(void *, void *, u16, u32);
void Anim_Step(void *, void *);
void AnimData_SelectPaletteBank(void *, u32);
void Sound_PlaySong(u32);
void SolarBank_BeginClosing(struct SolarBankPanel *);
void SolarBank_BeginSelection(struct SolarBankPanel *);
void SolarBank_RefreshAmountDigits(struct SolarBankPanel *);

#define FIELD(T, off) (*(T *)((u8 *)panel + (off)))
#define TRANSFER      FIELD(u32, 0x6F4)
#define BALANCE       FIELD(u32, 0x6F8)
#define BLINK         FIELD(u16, 0x700)
#define HOLD          FIELD(u16, 0x702)
#define SPEED         FIELD(u16, 0x704)
#define TIMER         FIELD(u16, 0x706)
#define DIRECTION     FIELD(u16, 0x708)

// MATCHING_CONCESSION: register-pinning
static inline u32 MaskHold(u32 value, u32 mask)
{
    asm("" : "+r"(mask));
    return mask & value;
}
#define SolarBank_UpdateTransferInput sub_081BB904
void SolarBank_UpdateTransferInput(struct SolarBankPanel *panel)
{
    s32 v;
    u16 t;

    if ((*(struct Keys *)gPadState).pressed & 1) {
        *(u32 *)(gGameState + 0x77C) = BALANCE;
        *(u16 *)(gGameState + 0x7B0) = TRANSFER;
        Sound_PlaySong(0xdd);
        SolarBank_BeginClosing(panel);
        goto step;
    }
    if ((*(struct Keys *)gPadState).pressed & 2) {
        Sound_PlaySong(0xde);
        SolarBank_BeginSelection(panel);
        goto step;
    }
    if ((*(struct Keys *)gPadState).held & 0x20) {
        u32 *bal = &BALANCE;
        u32 *xfer;
        u16 *hold;
        u16 *timer;
        u16 h;
        if (*bal > 9998)
            goto reset;
        xfer = &TRANSFER;
        if (*xfer == 0)
            goto reset;
        if (HOLD == 0)
            SPEED = 0;
        hold = &HOLD;
        DIRECTION = 0;
        timer = &TIMER;
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
            SPEED = 2;
            TIMER = 4;
        } else if (*hold > 0x3f) {
            if (!(MaskHold(h, 3)))
                Sound_PlaySong(0x37d);
            (*bal)++;
            (*xfer)--;
            SolarBank_RefreshAmountDigits(panel);
            SPEED = 1;
            *timer = 4;
            (*hold)++;
        } else {
            *hold = h + 1;
            if (!(MaskHold(h, 7))) {
                Sound_PlaySong(0x37d);
                (*bal)++;
                (*xfer)--;
                SolarBank_RefreshAmountDigits(panel);
            }
        }
    } else if ((t = (*(struct Keys *)gPadState).held & 0x10) != 0) {
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
        if (HOLD == 0)
            SPEED = 0;
        hold = &HOLD;
        DIRECTION = 1;
        timer = &TIMER;
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
            SPEED = 2;
            TIMER = 4;
        } else if (*hold > 0x3f) {
            if (!(MaskHold(h, 3)))
                Sound_PlaySong(0x37d);
            (*xfer)++;
            (*bal)--;
            SolarBank_RefreshAmountDigits(panel);
            SPEED = 1;
            *timer = 4;
            (*hold)++;
        } else {
            *hold = h + 1;
            if (!(MaskHold(h, 7))) {
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
    HOLD = 0;
    goto blink;
zero:
    HOLD = t;
blink:
    if (TIMER) {
        if (SPEED == 2)
            BLINK += 4;
        else if (SPEED == 1)
            BLINK += 2;
        else
            BLINK += 1;
        TIMER--;
    } else {
        BLINK = 0;
    }
    if (BLINK == 0) {
        AnimData_SelectPaletteBank((u8 *)panel + 0x6B0, 0x1AE);
        AnimData_SelectPaletteBank((u8 *)panel + 0x6CC, 0x1B1);
        Anim_LoadKeyframe(panel->unk1D8, panel->labelData, 0x12, 1);
    } else {
        switch ((BLINK & 0x1f) >> 3) {
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
        AnimData_SelectPaletteBank((u8 *)panel + 0x6B0, 0x1AE + v);
        AnimData_SelectPaletteBank((u8 *)panel + 0x6CC, 0x1B1 + v);
        Anim_LoadKeyframe(panel->unk1D8, panel->labelData, DIRECTION * 3 + 0x13 + v, 1);
    }
step:
    Anim_Step(&panel->label, panel->labelData);
}
