#include "global.h"
#include "render.h"
struct NumberDisplay {
    s32 *source;
    s32 value, maximum;
    u16 palette[16];
    s16 threshold, deltaR, deltaG, deltaB, timer;
    u16 unknown36, color;
    s16 phase;
    s32 red, green, blue;
};
void CpuSet(const void *, void *, u32);
void Palette_InitColorTransition(void *, s32, s32, s32, s32, s32, s32, s32);
u16 *Background_GetWrappedTileEntry(u32, s32, s32);
void FieldHud_UpdateNumberDisplay(struct NumberDisplay *p)
{
    s32 value, digit;
    u16 *tile;
    if (p->value != *p->source) {
        if (p->value > *p->source) {
            if (p->value - *p->source > 50)
                p->value -= 10;
            else
                p->value--;
        } else {
            if (*p->source - p->value > 50)
                p->value += 10;
            else
                p->value++;
        }
    }
    value = p->value;
    if (value > 999)
        value = 999;
    if (value < 0)
        value = 0;
    p->palette[7] = (p->red >> 4) | ((p->green >> 4) << 5) | ((p->blue >> 4) << 10);
    CpuSet(p->palette, gBgPaletteBank13, 8);
    if (value <= p->threshold) {
        p->red += p->deltaR;
        p->green += p->deltaG;
        p->blue += p->deltaB;
        if (p->timer == 0) {
            switch (p->phase) {
                case 0:
                    Palette_InitColorTransition(p, 4, 4, 4, 31, 0, 0, 20);
                    p->phase = 1;
                    break;
                case 1:
                    Palette_InitColorTransition(p, 31, 0, 0, 31, 31, 31, 10);
                    p->phase = 2;
                    break;
                case 2:
                    Palette_InitColorTransition(p, 31, 31, 31, 31, 0, 0, 10);
                    p->phase = 3;
                    break;
                case 3:
                    Palette_InitColorTransition(p, 31, 0, 0, 4, 4, 4, 20);
                    p->phase = 0;
                    break;
            }
        } else
            p->timer--;
    } else {
        p->red = 64;
        p->green = 64;
        p->blue = 64;
        p->phase = 0;
        p->timer = 0;
    }
    tile = Background_GetWrappedTileEntry(0, 3, 1);
    digit = 0;
    while (value > 99) {
        digit++;
        value -= 100;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 4, 1);
    digit = 0;
    while (value > 9) {
        digit++;
        value -= 10;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 5, 1);
    *tile = (u16)((value + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 6, 1);
    *tile = 0xd01c;
    value = p->maximum;
    tile = Background_GetWrappedTileEntry(0, 7, 1);
    digit = 0;
    while (value > 99) {
        digit++;
        value -= 100;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 8, 1);
    digit = 0;
    while (value > 9) {
        digit++;
        value -= 10;
    }
    *tile = (u16)((digit + 17) | 0xD000);
    tile = Background_GetWrappedTileEntry(0, 9, 1);
    *tile = (u16)((value + 17) | 0xD000);
}
