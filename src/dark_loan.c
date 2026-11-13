#include "dark_loan_screen.h"

void MessageWindow_SetPalette(u32);

void MessageWindow_SetMessageTable(u32);

void MessageWindow_SetBounds(s32, s32, s32, s32);

void MessageWindow_SetDisplayMode(s32);

s32 MessageWindow_ShowMessage(s32);

s32 MessageWindow_SetNumberArgument(s32, s32);

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

void Bg_SetLayerMap(u32, void *, u16);

void Bg_SetupLayers(int, int, void *, int, int, int, void *);

void StatGauge_Draw(s32, s32, s32, s32, s32, s32);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void Palette_UpdateHighlightPulse(u16 *, u8 *);

void sub_082157EC(void);

static inline void ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}

static inline void Position_Set(struct ContactPosition *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}

void DarkLoan_RelocateMapHeaderCopy(struct DarkLoanScreen *panel)
{
    struct DarkLoanMapHeader *src = panel->map;
    struct DarkLoanMapHeader h = *src;
    h.offsets[0] += (u32)src;
    h.offsets[1] += (u32)src;
    h.offsets[2] += (u32)src;
}

void DarkLoan_EnableAlternateRenderMode(struct DarkLoanScreen *panel)
{
    s32 i;
    u32 bit = 0x80;
    /* Advance through the widget run while retaining owner-relative flag access. */
    struct DarkLoanScreen *current = panel;
    for (i = 21; i >= 0; i--) {
        current->widgets[DARK_LOAN_BACKGROUND].flags |= bit;
        current = (void *)((u8 *)current + sizeof(struct SceneInputWidget));
    }
    panel->sprite.flags |= 0x80;
}

void DarkLoan_ShowPrompt(struct DarkLoanScreen *panel)
{
    MessageWindow_SetPalette(0x2E9);
    MessageWindow_SetMessageTable(panel->callbackPc);
    MessageWindow_SetBounds(1, 0xf, 0x1c, 4);
    MessageWindow_SetDisplayMode(1);
}

void DarkLoan_ResetBackground(struct DarkLoanScreen *panel)
{
    u32 zero = 0;
    Bg_SetupLayers(0, 0, panel->map, 0, 0, 1, &zero);
    Bg_SetLayerMap(0, panel->map, 0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    CpuSet(panel->procedureData, gBgPaletteBank13, 0x04000018);
}

void DarkLoan_UpdateBalanceDigits(struct DarkLoanScreen *panel)
{
    s32 digits[4];
    s32 value;
    s32 *p;
    struct DisplayResourceHeader *resource;
    struct SceneInputWidget *anim;
    s32 i;

    value = gGameState->menuRewardBalance + panel->adjustment * panel->rate;
    if (value == 0) {
        resource = &panel->secondaryResource;
        {
            s32 zero = 0;
            s32 *clear = &digits[3];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)digits);
        }
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
        resource = &panel->secondaryResource;
    }
    p = digits;
    anim = panel->widgets + DARK_LOAN_BALANCE_DIGITS;
    i = 3;
    do {
        Anim_LoadKeyframe(anim, resource, (u16)(*p++ + 20), 1);
        anim++;
    } while (--i >= 0);
}

void DarkLoan_UpdateAdjustmentDigits(struct DarkLoanScreen *display)
{
    s32 digits[4];
    s32 value, remaining, i;
    s32 *digit, *quantityDigit;
    void *data;
    u32 *base;
    struct SceneInputWidget *anim;
    u32 *flags;

    value = display->adjustment;
    if (value == 0) {
        base = &display->widgets[DARK_LOAN_BACKGROUND].flags;
        data = &display->resource;
        {
            s32 zero = 0;
            s32 *clear = &digits[3];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)digits);
        }
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
        base = &display->widgets[DARK_LOAN_BACKGROUND].flags;
        data = &display->resource;
    }
    digit = digits;
    anim = display->widgets + DARK_LOAN_ADJUSTMENT_DIGITS;
    flags = base + 10 * (sizeof(struct SceneInputWidget) / sizeof(*base));
    i = 3;
    do {
        *flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(*digit++ + 8), 1);
        anim++;
        flags = (void *)((u8 *)flags + sizeof(struct SceneInputWidget));
    } while (--i >= 0);
    display->widgets[DARK_LOAN_LOWER_RATE].flags &= ~1;
    value = display->adjustment * display->rate;
    if (value == 0) {
        {
            s32 zero = 0;
            s32 *clear = &digits[3];
            do {
                *clear-- = zero;
            } while ((s32)clear >= (s32)digits);
        }
    } else {
        digits[0] = Div(value, 1000);
        value -= digits[0] * 1000;
        digits[1] = Div(value, 100);
        value -= digits[1] * 100;
        digits[2] = Div(value, 10);
        digits[3] = value - digits[2] * 10;
    }
    quantityDigit = digits;
    anim = display->widgets + DARK_LOAN_AMOUNT_DIGITS;
    flags = base + 15 * (sizeof(struct SceneInputWidget) / sizeof(*base));
    i = 3;
    do {
        *flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(*quantityDigit++ + 8), 1);
        anim++;
        flags = (void *)((u8 *)flags + sizeof(struct SceneInputWidget));
    } while (--i >= 0);
}

void DarkLoan_DrawPreview(struct DarkLoanScreen *panel)
{
    s32 current, maximum;
    if (panel->exitScript == 0) {
        current = gPlayers[0]->currentStatB + panel->adjustment;
        maximum = gPlayers[0]->maximumStatB;
    } else {
        current = (s16)gGameState->unk2c + panel->adjustment;
        maximum = *(s16 *)gGameState->unk2E;
    }
    StatGauge_Draw(0, 2, 8, current, maximum, 0x35f);
}

void DarkLoan_HideSprites(struct DarkLoanScreen *panel)
{
    u32 one = 1;
    u32 *flags = &panel->widgets[DARK_LOAN_BACKGROUND].flags;
    s32 i = 0x15;
    do {
        *flags |= one;
        flags = (void *)((u8 *)flags + sizeof(struct SceneInputWidget));
        i--;
    } while (i >= 0);
    panel->sprite.flags |= 1;
}

void DarkLoan_SetState(struct DarkLoanScreen *panel, void (*update)(struct DarkLoanScreen *))
{
    panel->callback = update;
    panel->timer = 0;
}

void DarkLoan_BeginOpening(struct DarkLoanScreen *panel)
{
    u32 zero;
    DarkLoan_ResetBackground(panel);
    DarkLoan_ShowPrompt(panel);
    Bg_SetLayerMap(0, panel->map, 3);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    DarkLoan_HideSprites(panel);
    panel->widgets[DARK_LOAN_BACKGROUND].flags &= ~1;
    Anim_SetKeyframes(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource, 4, 2, zero);
    panel->widgets[DARK_LOAN_RATE].flags &= ~1;
    panel->widgets[DARK_LOAN_DECIMAL_DIGITS].flags &= ~1;
    panel->widgets[4].flags &= ~1;
    MessageWindow_ShowMessage(0);
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xe000, zero);
    if (!panel->exitScript)
        DarkLoan_FinishOpening(panel);
    else
        DarkLoan_SetState(panel, DarkLoan_UpdateOpeningAnimation);
}

void DarkLoan_FinishOpening(struct DarkLoanScreen *panel)
{
    u32 zero;
    Bg_SetLayerMap(0, panel->map, 3);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    DarkLoan_HideSprites(panel);
    panel->widgets[DARK_LOAN_BACKGROUND].flags &= ~1;
    Anim_SetKeyframes(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource, 7, 2, zero);
    panel->widgets[DARK_LOAN_RATE].flags &= ~1;
    panel->widgets[DARK_LOAN_DECIMAL_DIGITS].flags &= ~1;
    panel->widgets[4].flags &= ~1;
    panel->selection = 1;
    panel->widgets[DARK_LOAN_CURSOR].flags &= ~1;
    Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_CURSOR], &panel->secondaryResource, 0x4b, 1);
    MessageWindow_ShowMessage(1);
    panel->adjustment = zero;
    DarkLoan_SetState(panel, DarkLoan_UpdateConfirmation);
}

void DarkLoan_BeginAdjustment(struct DarkLoanScreen *panel)
{
    s32 i;
    Bg_SetLayerMap(0, panel->map, 4);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    DarkLoan_HideSprites(panel);
    panel->widgets[DARK_LOAN_BACKGROUND].flags &= ~1;
    Anim_SetKeyframes(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource, 5, 1, 0);
    panel->widgets[DARK_LOAN_PERCENT].flags &= ~1;
    Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_PERCENT], &panel->resource, 18, 1);
    panel->adjustment = 0;
    for (i = 0; i < 4; i++) {
        panel->widgets[DARK_LOAN_BALANCE_DIGITS + i].flags &= ~1;
    }
    DarkLoan_UpdateBalanceDigits(panel);
    DarkLoan_DrawPreview(panel);
    panel->widgets[DARK_LOAN_LOWER_RATE].flags &= ~1;
    for (i = 0; i < 4; i++) {
        panel->widgets[DARK_LOAN_ADJUSTMENT_DIGITS + i].flags &= ~1;
        panel->widgets[DARK_LOAN_AMOUNT_DIGITS + i].flags &= ~1;
    }
    DarkLoan_UpdateAdjustmentDigits(panel);
    panel->warningTimer = 0;
    panel->warningShowing = 0;
    ShowWidget(&panel->widgets[DARK_LOAN_LABEL]);
    panel->sprite.flags &= ~1;
    AnimData_SelectPaletteBank(panel->animation, 0x1B4);
    ShowWidget(&panel->widgets[DARK_LOAN_DECORATIONS]);
    ShowWidget(&panel->widgets[21]);
    MessageWindow_ShowMessage(4);
    panel->adjustmentState[1] = 0;
    panel->adjustmentState[2] = 0;
    panel->changeFrame = 0;
    panel->changeKeyframe = 0;
    panel->adjustmentState[0] = 0;
    DarkLoan_SetState(panel, DarkLoan_UpdateAdjustment);
}

void DarkLoan_BeginClosing(struct DarkLoanScreen *panel)
{
    int z;
    Bg_SetLayerMap(0, panel->map, 5);
    z = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, z);
    DarkLoan_HideSprites(panel);
    Sound_PlaySong(0x2c1);
    panel->widgets[DARK_LOAN_BACKGROUND].flags &= ~1;
    Anim_SetKeyframes(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource, 6, 2, z);
    MessageWindow_ShowMessage(3);
    DarkLoan_SetState(panel, DarkLoan_WaitBeforeConfirmation);
}

void DarkLoan_ShowCompletionMessage(struct DarkLoanScreen *panel)
{
    Bg_SetLayerMap(0, panel->map, 5);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    DarkLoan_HideSprites(panel);
    panel->widgets[DARK_LOAN_BACKGROUND].flags &= ~1;
    if (*(s16 *)&gGameState->unk524[0x10] <= 3)
        MessageWindow_ShowMessage(9);
    else if (*(s16 *)&gGameState->unk524[0x10] <= 5)
        MessageWindow_ShowMessage(0xa);
    else if (*(s16 *)&gGameState->unk524[0x10] <= 7)
        MessageWindow_ShowMessage(0xb);
    else
        MessageWindow_ShowMessage(0xc);
    DarkLoan_SetState(panel, DarkLoan_WaitBeforeFade);
}

void DarkLoan_BeginAlternateOpening(struct DarkLoanScreen *panel)
{
    u32 zero;
    DarkLoan_ResetBackground(panel);
    DarkLoan_ShowPrompt(panel);
    Bg_SetLayerMap(0, panel->map, 5);
    zero = 0;
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    DarkLoan_HideSprites(panel);
    panel->widgets[DARK_LOAN_BACKGROUND].flags &= ~1;
    Anim_SetKeyframes(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource, 6, 2, zero);
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xe000, zero);
    MessageWindow_ShowMessage(13);
    DarkLoan_SetState(panel, DarkLoan_UpdateClosing);
}

void DarkLoan_HideDisplay(struct DarkLoanScreen *panel)
{
    DarkLoan_HideSprites(panel);
    DarkLoan_SetState(panel, DarkLoan_UpdateHidden);
}

void DarkLoan_UpdateHidden(struct DarkLoanScreen *panel)
{
}

void DarkLoan_UpdateOpeningAnimation(struct DarkLoanScreen *panel)
{
    u16 *c;
    Anim_Step(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource);
    c = &panel->timer;
    if (*c == 0x20)
        Sound_PlaySong(0x2c0);
    (*c)++;
    if (*c > 0x68)
        DarkLoan_FinishOpening(panel);
}

void DarkLoan_ConfirmPreview(struct DarkLoanScreen *panel)
{
    if (panel->adjustment != 0) {
        MessageWindow_ShowMessage(5);
        if (panel->confirmScript != 0)
            Script_Call(panel->confirmScript, 0);
        else
            MessageWindow_SetNumberArgument(0, 3);
        DarkLoan_SetState(panel, DarkLoan_WaitAfterConfirm);
    } else {
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
        DarkLoan_SetState(panel, DarkLoan_CloseAfterDelay);
    }
}

void DarkLoan_UpdateConfirmation(struct DarkLoanScreen *panel)
{
    Anim_Step(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource);
    Palette_UpdateHighlightPulse(
        ((struct DarkLoanHighlightPalette *)panel->widgets[DARK_LOAN_CURSOR].palette)->colors,
        &panel->highlightPulse);
    if (gPadState[1] & 1) {
        if (panel->selection) {
            Sound_PlaySong(0xdd);
            DarkLoan_BeginAdjustment(panel);
        } else {
            Sound_PlaySong(0xde);
            if (!panel->exitScript)
                DarkLoan_ConfirmPreview(panel);
            else
                DarkLoan_BeginClosing(panel);
        }
    } else if (gPadState[1] & 2) {
        Sound_PlaySong(0xde);
        if (!panel->exitScript)
            DarkLoan_ConfirmPreview(panel);
        else
            DarkLoan_BeginClosing(panel);
    } else if (gPadState[1] & 0x30) {
        Sound_PlaySong(0xdc);
        panel->selection = 1 - panel->selection;
        if (panel->selection)
            Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_CURSOR], &panel->secondaryResource, 0x4b,
                              1);
        else
            Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_CURSOR], &panel->secondaryResource, 0x4c,
                              1);
    }
}

s32 DarkLoan_GetSelectionGroup(s32 selection)
{
    s32 group;
    switch (selection >> 2) {
        case 2:
            group = 2;
            break;
        case 1:
        case 3:
            group = 1;
            break;
        case 0:
        default:
            group = 0;
            break;
    }
    return group;
}

void DarkLoan_UpdateAdjustment(struct DarkLoanScreen *panel)
{
    u16 *balance, *limit;
    u32 group;
    s32 total;
    u32 *p, *q;
    s32 i;
    Anim_Step(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource);
    if (!panel->exitScript) {
        balance = &gPlayers[0]->currentStatB;
        limit = &gPlayers[0]->maximumStatB;
    } else {
        balance = &gGameState->unk2c;
        limit = (u16 *)gGameState->unk2E;
    }
    if (gPadState[1] & 1) {
        if (!panel->adjustment)
            Sound_PlaySong(0xDE);
        else {
            *balance += panel->adjustment;
            /* The balance update is unsigned; the following lifetime total saturates. */
            *(u32 *)&gGameState->menuRewardBalance += panel->adjustment * panel->rate;
            total = gGameState->unk51C + panel->adjustment * panel->rate;
            if (total < 0)
                total = 0x7FFFFFFF;
            gGameState->unk51C = total;
            Sound_PlaySong(0xDD);
        }
        DarkLoan_BeginClosing(panel);
        return;
    }
    if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        DarkLoan_FinishOpening(panel);
        return;
    }
    if ((gPadState[0] & 0x20) &&
        gGameState->menuRewardBalance + (panel->adjustment + 1) * panel->rate <= 9999 &&
        *balance + panel->adjustment < *limit) {
        Sound_PlaySong(0x37D);
        panel->adjustment++;
        DarkLoan_UpdateBalanceDigits(panel);
        DarkLoan_DrawPreview(panel);
        DarkLoan_UpdateAdjustmentDigits(panel);
        panel->changeKeyframe = 19;
        group = DarkLoan_GetSelectionGroup(panel->changeFrame);
        Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_PERCENT], &panel->resource,
                          (u16)(panel->changeKeyframe + group), 1);
        AnimData_SelectPaletteBank(&panel->animation, group + 0x1B4);
        if (++panel->changeFrame > 15)
            panel->changeFrame = 0;
    } else if ((gPadState[0] & 0x10) && panel->adjustment) {
        Sound_PlaySong(0x37D);
        panel->adjustment--;
        DarkLoan_UpdateBalanceDigits(panel);
        DarkLoan_DrawPreview(panel);
        DarkLoan_UpdateAdjustmentDigits(panel);
        panel->changeKeyframe = 22;
        group = DarkLoan_GetSelectionGroup(panel->changeFrame);
        Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_PERCENT], &panel->resource,
                          (u16)(panel->changeKeyframe + group), 1);
        AnimData_SelectPaletteBank(&panel->animation, group + 0x1B4);
        if (++panel->changeFrame > 15)
            panel->changeFrame = 0;
        if (panel->warningTimer)
            panel->warningTimer = 1;
    } else if (panel->changeFrame) {
        group = DarkLoan_GetSelectionGroup(panel->changeFrame);
        Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_PERCENT], &panel->resource,
                          (u16)(panel->changeKeyframe + group), 1);
        AnimData_SelectPaletteBank(&panel->animation, group + 0x1B4);
        if (++panel->changeFrame > 15)
            panel->changeFrame = 0;
    } else {
        AnimData_SelectPaletteBank(&panel->animation, 0x1B4);
        Anim_LoadKeyframe(&panel->widgets[DARK_LOAN_PERCENT], &panel->resource, 18, 1);
    }
    if ((gPadState[0] & 0x20) &&
        gGameState->menuRewardBalance + (panel->adjustment + 1) * panel->rate > 9999)
        panel->warningTimer = 60;
    if (panel->warningTimer) {
        if (--panel->warningTimer == 0) {
            DarkLoan_UpdateAdjustmentDigits(panel);
            MessageWindow_ShowMessage(4);
            panel->warningShowing = 0;
        } else {
            u32 mask;
            panel->widgets[DARK_LOAN_LOWER_RATE].flags |= 1;
            mask = 1;
            q = (u32 *)(&panel->widgets[DARK_LOAN_AMOUNT_DIGITS].flags);
            p = (u32 *)(&panel->widgets[DARK_LOAN_ADJUSTMENT_DIGITS].flags);
            i = 3;
            do {
                *p |= mask;
                *q |= mask;
                q = (void *)((u8 *)q + sizeof(struct SceneInputWidget));
                p = (void *)((u8 *)p + sizeof(struct SceneInputWidget));
            } while (--i >= 0);
            if (!panel->warningShowing) {
                MessageWindow_ShowMessage(6);
                panel->warningShowing = 1;
            }
        }
    }
}

void DarkLoan_WaitBeforeConfirmation(struct DarkLoanScreen *panel)
{
    Anim_Step(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource);
    if (++panel->timer > 149) {
        if (panel->adjustment != 0) {
            MessageWindow_ShowMessage(5);
            if (panel->confirmScript != 0)
                Script_Call(panel->confirmScript, 0);
            else
                MessageWindow_SetNumberArgument(0, 3);
            DarkLoan_SetState(panel, DarkLoan_WaitAfterConfirm);
        } else {
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
            DarkLoan_SetState(panel, DarkLoan_CloseAfterDelay);
        }
    }
}

void DarkLoan_WaitAfterConfirm(struct DarkLoanScreen *panel)
{
    if (++panel->timer > 149) {
        if (*(s16 *)&gGameState->unk524[0x10] == 0) {
            ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
            DarkLoan_SetState(panel, DarkLoan_CloseAfterDelay);
        } else {
            DarkLoan_ShowCompletionMessage(panel);
        }
    }
}

void DarkLoan_WaitBeforeFade(struct DarkLoanScreen *panel)
{
    if (++panel->timer > 149) {
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
        DarkLoan_SetState(panel, DarkLoan_CloseAfterDelay);
    }
}

void DarkLoan_CloseAfterDelay(struct DarkLoanScreen *panel)
{
    if (++panel->timer > 31) {
        if (panel->exitScript == 0) {
            u16 *flags = &gDispCntShadow;
            u32 mask = ~0x100;
            *flags = mask & *flags;
            DarkLoan_HideDisplay(panel);
        } else {
            Script_Call(panel->exitScript, 0);
            Task_Kill(panel);
            sub_082157EC();
            {
                u16 *flags = &gDispCntShadow;
                u32 mask = ~0x100;
                *flags = mask & *flags;
            }
        }
    }
}

void DarkLoan_UpdateClosing(struct DarkLoanScreen *panel)
{
    Anim_Step(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource);
    if (panel->timer == 150)
        ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xe000, 0);
    if (++panel->timer > 181) {
        if (panel->exitScript == 0) {
            u16 *flags = &gDispCntShadow;
            u32 mask = ~0x100;
            *flags = mask & *flags;
            DarkLoan_HideDisplay(panel);
        } else {
            Script_Call(panel->exitScript, 0);
            Task_Kill(panel);
        }
    }
}

s32 DarkLoan_OnUpdate(struct DarkLoanScreen *panel)
{
    Callback_InvokeActor(panel, panel->callback);
    return 0;
}

s32 DarkLoan_OnDestroy(struct DarkLoanScreen *panel)
{
    struct SceneInputWidget *widget = panel->widgets;
    s32 i = 0x15;
    do {
        Anim_Remove(widget);
        widget++;
        i--;
    } while (i >= 0);
    Sprite_Remove(&panel->sprite);
    return 0;
}

void DarkLoan_StartDisplayScript(struct DarkLoanScreen *panel)
{
    u16 *r = &gDispCntShadow;
    u32 m = ~0x100;
    *r = m & *r;
    panel->map = ScriptProc_Start(0xC091, 0x8544);
}

void DarkLoan_InitializeProcedure(struct DarkLoanScreen *panel)
{
    panel->procedureData = (u8 *)ScriptProc_Start(0x92B3, 0x2E9) + 0x1B4;
    CpuSet(gPaletteBankData + 0x7540, panel->snapshot, 0x04000008);
    panel->highlightPulse = 0;
}

void DarkLoan_BuildDisplay(struct DarkLoanScreen *panel, s32 alternate)
{
    struct DisplayResourceHeader *res;
    struct DisplayResourceHeader *savedSecondary;
    struct DisplayResourceHeader *primary;
    struct DisplayResourceHeader *secondary;
    struct ContactPosition pos;
    s32 flags;
    struct SceneWidgetPlayback backgroundPlayback;
    u8 backgroundDefault;
    s32 i;

    res = ScriptProc_Start(0xCB05, 0x32BA);
    panel->resource = *res;
    ResourceHeader_Relocate(&panel->resource, res);
    res = ScriptProc_Start(0xCB05, 0x530D);
    panel->secondaryResource = *res;
    savedSecondary = &panel->secondaryResource;
    ResourceHeader_Relocate(savedSecondary, res);
    backgroundPlayback.priority = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    backgroundDefault = backgroundPlayback.priority;
    flags = 0x10;
    if (alternate)
        flags = 0x90;
    Widget_InitLinked(&panel->widgets[DARK_LOAN_BACKGROUND], &panel->resource, 4, flags,
                      backgroundDefault, backgroundDefault, 0x3C, &pos);
    Widget_InitLinked(&panel->widgets[DARK_LOAN_RATE], &panel->resource, (u16)(panel->rate + 7),
                      flags, 0, 0, 0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&panel->widgets[DARK_LOAN_DECIMAL_DIGITS], &panel->resource, 8, flags, 0, 0,
                      0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&panel->widgets[4], &panel->resource, 8, flags, 0, 0, 0x3C, &pos);
    pos.x = -8;
    pos.y = -8;
    Widget_InitLinked(&panel->widgets[DARK_LOAN_CURSOR], savedSecondary, 0x4B, flags, 0, 0, 0x3C,
                      &pos);
    panel->widgets[DARK_LOAN_CURSOR].palette = (u16 *)panel->snapshot;
    pos.x = 0x10;
    pos.y = -4;
    Widget_InitLinked(&panel->widgets[DARK_LOAN_PERCENT], &panel->resource, 0x12, flags, 0, 0, 0x3C,
                      &pos);
    pos.x = 0x90;
    pos.y = 0x28;
    primary = &panel->resource;
    secondary = savedSecondary;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &panel->widgets[DARK_LOAN_BALANCE_DIGITS + i];
        Widget_InitLinked(widget, secondary, 0x14, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    pos.x = -0x38;
    pos.y = 0x38;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &panel->widgets[DARK_LOAN_ADJUSTMENT_DIGITS + i];
        Widget_InitLinked(widget, primary, 8, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    Widget_InitLinked(&panel->widgets[DARK_LOAN_LABEL], primary, 0x22, flags, 0, 0, 0x3C, 0);
    Position_Set(&pos, -0x85, -0x18, 0);
    Widget_InitLinked(&panel->widgets[DARK_LOAN_DECORATIONS], secondary, 0x7E, flags, 0, 0, 0x3C,
                      &pos);
    Widget_InitLinked(&panel->widgets[21], secondary, 0x7F, flags, 0, 0, 0x3C, &pos);
    pos.x = 0x58;
    pos.y = 0x38;
    Widget_InitLinked(&panel->widgets[DARK_LOAN_LOWER_RATE], primary, (u16)(panel->rate + 7), flags,
                      0, 0, 0x3C, &pos);
    pos.x = -0x30;
    pos.y = 0x48;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &panel->widgets[DARK_LOAN_AMOUNT_DIGITS + i];
        Widget_InitLinked(widget, primary, 8, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    AnimData_Load(panel->animation, 0xA47C);
    Sprite_Init(&panel->sprite, panel->animation, flags);
    panel->sprite.priority = 0;
    AnimData_SelectPaletteBank(panel->animation, 0x1B4);
    panel->sprite.position.x = 0xA0;
    panel->sprite.position.y = 0x58;
    panel->sprite.position.z = 0;
}

s32 DarkLoan_GetDefaultRate(void)
{
    s16 v = gGameState->unk876;
    s32 r;
    if (v <= 1)
        r = 9;
    else if (v <= 9)
        r = 11 - v;
    else
        r = 2;
    return r;
}

s32 DarkLoan_InitializeFromScript(struct DarkLoanScreen *panel)
{
    u32 callbackPc;
    if (Script_SeekToKeyword(0x52)) {
        panel->rate = Script_GetValue();
        if (panel->rate <= 1)
            panel->rate = 2;
        if (panel->rate > 9)
            panel->rate = 9;
    } else {
        panel->rate = DarkLoan_GetDefaultRate();
    }
    if (Script_SeekToKeyword(0x64))
        panel->confirmScript = Script_GetValue();
    else
        panel->confirmScript = 0;
    if (Script_SeekToKeyword(0x65))
        panel->exitScript = Script_GetValue();
    else
        panel->exitScript = 0;
    if (Script_SeekToKeyword(0x6F))
        panel->displayMode = Script_GetValue();
    else
        panel->displayMode = 0;
    if (Script_SeekToKeyword(0x72))
        callbackPc = Script_GetTextSourcePc();
    else
        callbackPc = 0;
    DarkLoan_StartDisplayScript(panel);
    DarkLoan_InitializeProcedure(panel);
    DarkLoan_BuildDisplay(panel, 0);
    panel->callbackPc = callbackPc;
    if (panel->displayMode)
        DarkLoan_BeginAlternateOpening(panel);
    else
        DarkLoan_BeginOpening(panel);
    return 0;
}

struct DarkLoanScreen *DarkLoan_Create(void)
{
    struct DarkLoanScreen *r = Task_Create(0xb, sizeof(struct DarkLoanScreen));
    if (r != 0) {
        Task_SetCallbacks(r, (void (*)(void *))DarkLoan_OnUpdate,
                          (void (*)(void *))DarkLoan_OnDestroy);
        if (DarkLoan_InitializeFromScript(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

struct DarkLoanScreen *DarkLoan_CreateHiddenDisplay(u32 rate, s32 confirmScript, u32 displayMode,
                                                    u32 callbackPc)
{
    struct DarkLoanScreen *panel;
    u16 *rateField;
    panel = Task_Create(0xB, sizeof(struct DarkLoanScreen));
    if (panel)
        Task_SetCallbacks(panel, (void (*)(void *))DarkLoan_OnUpdate,
                          (void (*)(void *))DarkLoan_OnDestroy);
    do {
        rateField = &panel->rate;
        *rateField = rate;
        if (rate - 2 > 7)
            *rateField = DarkLoan_GetDefaultRate();
    } while (0);
    DarkLoan_InitializeProcedure(panel);
    DarkLoan_BuildDisplay(panel, 1);
    panel->callbackPc = callbackPc;
    panel->confirmScript = confirmScript;
    panel->exitScript = 0;
    panel->displayMode = displayMode;
    DarkLoan_HideDisplay(panel);
    return panel;
}

void DarkLoan_OpenDisplay(struct DarkLoanScreen *panel)
{
    DarkLoan_StartDisplayScript(panel);
    if (panel->displayMode)
        DarkLoan_BeginAlternateOpening(panel);
    else
        DarkLoan_BeginOpening(panel);
}
