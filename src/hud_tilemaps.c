#include "scene_hud_map.h"
#include "render.h"
#include "scene_hud_display.h"
#include "scene_input_panel.h"

u16 *SceneHud_GetMapCell(u32 layerIndex, u32 x, u32 y)
{
    struct BackgroundLayer *layer = &gBgRecords[layerIndex];
    u8 *base = (u8 *)layer->map;
    return (u16 *)(base + ((x & 31) << 1) + ((y & 31) << 6));
}

void SceneHud_FillTile001(u32 x, u32 y, s32 count)
{
    u16 *cell = SceneHud_GetMapCell(0, x, y);
    if (count > 0) {
        do {
            *cell++ = 0xF001;
            count--;
        } while (count > 0);
    }
}

void SceneHud_FillTile020(u32 x, u32 y, s32 count)
{
    u16 *cell = SceneHud_GetMapCell(0, x, y);
    s32 remaining;
    for (remaining = count; remaining > 0; remaining--)
        *cell++ = 0xF020;
}

static inline void DrawDecimalDigits(u16 *cell, s32 value, s32 divisor, s32 style)
{
    s32 base = (style << 4) + 0x21;
    if (divisor > 0) {
        do {
            s32 digit = Div(value, divisor);
            u32 glyph = (u16)(digit + base);
            *cell++ = glyph | 0xF000;
            value -= digit * divisor;
            divisor = Div(divisor, 10);
        } while (divisor > 0);
    }
}

void SceneHud_DrawDecimal(s32 value, s32 divisor, u32 x, u32 y, s32 style)
{
    u16 *cell = SceneHud_GetMapCell(0, x, y);
    DrawDecimalDigits(cell, value, divisor, style);
}

void SceneHud_DrawCenteredMeter(s32 count)
{
    u16 *cell = SceneHud_GetMapCell(0, 5, 2);
    s32 i = 0;
    s32 start = gUnk_0824DEB8[count - 1];
    s32 end = start + count;
    const u16 *empty = gUnk_0824DED0;
    u32 offset = 0;
    const u16 *selected = gUnk_0824DEC0;
    do {
        u16 tile;
        if ((i >= start) && (i < end)) {
            tile = *(const u16 *)(offset + (u32)selected);
        } else {
            tile = *empty;
        }
        do {
            *cell++ = tile;
            empty++;
        } while (0);
        offset += 2;
        i++;
    } while (i <= 7);
}

void SceneHud_DrawClock(u32 mode)
{
    u16 *output = SceneHud_GetMapCell(0, 20, 0);
    if (mode == 0) {
        *output++ = 0xF0BD;
        output[0] = 0xF0B1;
        output[1] = 0xF0BB;
        SceneHud_DrawDecimal(SunClock_GetBaseHour(), 10, 24, 0, mode);
        SceneHud_DrawDecimal(SunClock_GetBaseMinute(), 10, 27, 0, mode);
    } else if (mode == 1) {
        *output++ = 0xF07C;
        output[0] = 0xF07D;
        output[1] = 0xF07E;
        SceneHud_DrawDecimal(gSunClock.hourA, 10, 24, 0, 0);
        SceneHud_DrawDecimal(gSunClock.minuteA, 10, 27, 0, 0);
    } else {
        *output++ = 0xF07C;
        output[0] = 0xF07D;
        output[1] = 0xF07F;
        SceneHud_DrawDecimal(gSunClock.hourB, 10, 24, 0, 0);
        SceneHud_DrawDecimal(gSunClock.minuteB, 10, 27, 0, 0);
    }
}

void SceneHud_DrawNumericStatus(u32 mode)
{
    u16 *output = SceneHud_GetMapCell(0, 20, 1);
    if (mode == 0) {
        *output++ = 0xF0C0;
        *output++ = 0xF0BB;
        *output++ = 0xF0B2;
        output[0] = 0xF061;
        output[1] = 0xF001;
        SceneHud_DrawDecimal(gGameState->unk7B0, 10000, 24, 1, mode);
    } else if (mode == 1) {
        *output++ = 0xF0BA;
        *output++ = 0xF0D0;
        *output++ = 0xF0BB;
        output[0] = 0xF061;
        output[1] = 0xF020;
        SceneHud_DrawDecimal(gGameState->menuRewardBalance, 10000, 24, 1, mode);
    } else {
        *output++ = 0xF0B5;
        *output++ = 0xF0B6;
        *output++ = 0xF0BB;
        output[0] = 0xF0BF;
        output[1] = 0xF061;
        SceneHud_DrawDecimal(gGameState->unk77C, 10000, 24, 1, 0);
    }
}

void SceneHud_SetKeyframePaletteBank(struct SceneInputWidget *target, u32 bank)
{
    target->paletteBank = bank;
    target->palette = (u16 *)(gPaletteBankData + (target->paletteBank << 5));
}

/* SceneHud_UpdateFeaturePalettes: Sets up to three widget palette banks to 0x374 or 0x373 according to owner flag masks 4/8/16; preserves the existing provisional alias */

void SceneHud_UpdateFeaturePalettes(const void *owner, struct SceneInputWidget *first,
                                    struct SceneInputWidget *second, struct SceneInputWidget *third)
{
    if (first) {
        if (SceneInput_TestGridFlags(owner, 4))
            SceneHud_SetKeyframePaletteBank(first, 0x374);
        else
            SceneHud_SetKeyframePaletteBank(first, 0x373);
    }
    if (second) {
        if (SceneInput_TestGridFlags(owner, 8))
            SceneHud_SetKeyframePaletteBank(second, 0x374);
        else
            SceneHud_SetKeyframePaletteBank(second, 0x373);
    }
    if (third) {
        if (SceneInput_TestGridFlags(owner, 16))
            SceneHud_SetKeyframePaletteBank(third, 0x374);
        else
            SceneHud_SetKeyframePaletteBank(third, 0x373);
    }
}

void SceneHud_ShowFourDigits(struct SceneInputPanel *parent, const struct ContactPosition *position,
                             s32 value, u32 label)
{
    void *resource = &parent->widgetResource;
    struct SceneInputWidget *widget = &parent->numericLabel;
    s32 digits[4];
    s32 offset, remaining;
    s32 *digit;
    widget->flags &= ~1;
    widget->position = *position;
    Anim_LoadKeyframe(widget, resource, (u16)(label + 0xA8), 1);
    widget = parent->smallDigits;
    digits[0] = Div(value, 1000);
    value -= digits[0] * 1000;
    digits[1] = Div(value, 100);
    value -= digits[1] * 100;
    digits[2] = Div(value, 10);
    value -= digits[2] * 10;
    digits[3] = value;
    offset = 0;
    digit = digits;
    remaining = 3;
    do {
        widget->flags &= ~1;
        Anim_LoadKeyframe(widget, resource, (u16)(*digit++ + 0xAA), 1);
        widget->position = *position;
        widget->position.x += offset;
        widget++;
        offset += 6;
        remaining--;
    } while (remaining >= 0);
}

void SceneHud_HideFourDigits(struct SceneInputPanel *parent)
{
    struct SceneInputWidget *widget = &parent->numericLabel;
    u32 hidden = 1;
    s32 remaining = 4;
    do {
        widget->flags |= hidden;
        widget++;
    } while (--remaining >= 0);
}
