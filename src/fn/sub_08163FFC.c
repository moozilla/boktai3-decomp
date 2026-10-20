#include "global.h"
#define StatDisplay_DrawRatio sub_08163FFC
s32 Div(s32, s32);
u16 *Hud_GetTilemapCell(s32, s32, s32);
u32 Hud_GetStyleTileIndex(s32, s32);
void StatDisplay_DrawRatio(void *unused, s32 style, s32 divisor, s32 count, s32 numerator,
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
