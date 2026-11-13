#include "hud_stat_display.h"

void PauseMenu_LoadSpriteKeyframe(struct SceneHudState *, s32, s32);

void Hud_SetUpdateCallback(struct SceneHudState *hud, u32 callback)
{
    hud->updateCallback = (void (*)(struct SceneHudState *))callback;
    hud->pageFadeTimer = 0;
}

void Hud_SetInteractionCallback(struct SceneHudState *hud, u32 callback, u32 comparison)
{
    hud->pageCallback = (void *)callback;
    hud->actionTransferDelay = 0;
    hud->comparisonModifier = comparison;
    if ((comparison << 24) != 0) {
        PauseMenu_LoadSpriteKeyframe(hud, 1, 0x43);
        PauseMenu_LoadSpriteKeyframe(hud, 2, 0x44);
    } else {
        PauseMenu_LoadSpriteKeyframe(hud, 1, 0x41);
        PauseMenu_LoadSpriteKeyframe(hud, 2, 0x42);
    }
}

void Hud_SetupBackgroundLayer(s32 unused, void *map, s32 offset, s32 format, const void *palette)
{
    u32 initialScroll = 0;
    Bg_SetupLayers(0, (s16)offset, map, 0, 0, 1, &initialScroll);
    Bg_SetLayerMap(0, map, format);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    CpuSet(palette, (void *)0x03005160, 0x04000018);
}

u16 *Hud_GetTilemapCell(s32 layer, u32 x, u32 y)
{
    struct BackgroundLayer *background;
    u16 *cell;
    u32 coordinateMask;
    background = &gBgRecords[layer];
    cell = background->map;
    coordinateMask = 0x1f;
    cell += x & coordinateMask;
    cell = (u16 *)((u8 *)cell + ((y & coordinateMask) << 6));
    return cell;
}

s32 Hud_GetStyleTileIndex(s32 digit, s32 style)
{
    s32 baseTile = 0x11;
    if (style != 0) {
        baseTile = 0x31;
        if (style == 1)
            baseTile = 0x21;
    }
    return digit + baseTile;
}

void Hud_InitSelectionBounds(u32 level, u32 color)
{
    gBgFadeColor = color;
    gBgFadeBankMask = 0xFFFF;
    gBgFadeLevel = level;
    gObjFadeColor = color;
    gObjFadeFlags = 0;
    gObjFadeLevel = level;
}

s32 Hud_GetMinimum(s32 first, s32 second)
{
    s32 minimum = second;
    if (first <= minimum)
        minimum = first;
    return minimum;
}

void Hud_DrawStatRatio(void *unused, s32 style, s32 divisor, s32 count, s32 numerator,
                       s32 denominator, s32 x, s32 y)
{
    s32 arraySize = 5;
    s32 first[arraySize];
    s32 second[arraySize];
    s32 value, place, i;
    s32 tile, slash, trailing, digitStyle;
    u32 palette;
    u16 *cell;
    value = numerator;
    place = divisor;
    for (i = 0; i < count; i++) {
        first[i] = Div(value, place);
        value -= first[i] * place;
        place = Div(place, 10);
    }
    value = denominator;
    place = divisor;
    for (i = 0; i < count; i++) {
        second[i] = Div(value, place);
        value -= second[i] * place;
        place = Div(place, 10);
    }
    if (style == 0) {
        tile = 0;
        slash = 28;
        digitStyle = 0;
        trailing = 3;
        palette = 0xD000;
        cell = Hud_GetTilemapCell(0, x, y);
    } else {
        tile = 1;
        slash = 97;
        digitStyle = 1;
        trailing = 2;
        if (count == 3)
            trailing = 4;
        palette = 0xF000;
        cell = Hud_GetTilemapCell(0, x - 1, y);
    }
    *cell++ = palette | tile;
    for (i = 0; i < count; i++)
        *cell++ = palette | Hud_GetStyleTileIndex(first[i], digitStyle);
    *cell++ = palette | slash;
    for (i = 0; i < count; i++)
        *cell++ = palette | Hud_GetStyleTileIndex(second[i], digitStyle);
    for (i = 0; i < trailing; i++)
        *cell++ = palette | tile;
}

void Hud_DrawHealthRatio(struct SceneHudState *hud, u32 style, u32 x, u32 y)
{
    u32 first = hud->actionOwner->currentStatA <= 9999 ? hud->actionOwner->currentStatA : 9999;
    u32 second = hud->actionOwner->maximumStatA <= 9999 ? hud->actionOwner->maximumStatA : 9999;
    Hud_DrawStatRatio(hud, style, 1000, 4, first, second, x, y);
}

void Hud_DrawEnergyRatio(struct SceneHudState *hud, u32 style, u32 x, u32 y)
{
    u32 first = hud->actionOwner->currentStatB <= 999 ? hud->actionOwner->currentStatB : 999;
    u32 second = hud->actionOwner->maximumStatB <= 999 ? hud->actionOwner->maximumStatB : 999;
    Hud_DrawStatRatio(hud, style, 100, 3, first, second, x, y);
}

void Hud_DrawPlayerValue(struct SceneHudState *hud, s32 style, s32 x, s32 y)
{
    if (hud->playerValueEnabled) {
        if ((u16)((u16)gGameState->form - 1) <= 1) {
            s32 current = Div(hud->actionOwner->statusValue, 4);
            s32 maximum = Div(4000, 4);
            Hud_DrawStatRatio(hud, style, 1000, 4, current, maximum, x, y);
        } else {
            s32 divisor = 1000;
            Hud_DrawStatRatio(hud, style, divisor, 4, hud->actionOwner->statHundredths, divisor, x,
                              y);
        }
    }
}
