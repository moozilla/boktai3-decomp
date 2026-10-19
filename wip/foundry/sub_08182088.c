#include "global.h"

#define ArmorPalette_PulseAccentColor sub_08182088

struct ArmorPaletteController {
    void (*update)(void *);
    void (*finish)(void *);
    u16 source[16][16]; /* 0x008 */
    u16 banks[16][16];  /* 0x208 */
    u16 saved[4][16];   /* 0x408 */
    s16 timer;          /* 0x488 */
    u8 bank;            /* 0x48a */
    u8 otherBank;       /* 0x48b */
};

void sub_08182088(struct ArmorPaletteController *c)
{
    s32 color;
    s32 green, blue, red;
    s32 scale;

    if (c->timer <= 0x1f) {
        color = c->source[c->bank + 1][1];
        green = (color >> 5) & 0x1f;
        blue = (color >> 10) & 0x1f;
        red = color & 0x1f;
        scale = 0x20 - c->timer;
        red = red * scale >> 5;
        green = green * scale >> 5;
        blue = blue * scale >> 5;
        c->banks[c->bank + 1][1] = red | green << 5 | blue << 10;
        color = c->source[c->bank + 2][1];
        green = (color >> 5) & 0x1f;
        blue = (color >> 10) & 0x1f;
        red = color & 0x1f;
        scale = 0x20 - c->timer;
        red = red * scale >> 5;
        green = green * scale >> 5;
        blue = blue * scale >> 5;
        c->banks[c->bank + 2][1] = red | green << 5 | blue << 10;
    } else {
        color = c->source[c->bank + 1][1];
        green = (color >> 5) & 0x1f;
        blue = (color >> 10) & 0x1f;
        red = color & 0x1f;
        scale = c->timer - 0x20;
        red = red * scale >> 5;
        green = green * scale >> 5;
        blue = blue * scale >> 5;
        c->banks[c->bank + 1][1] = red | green << 5 | blue << 10;
        color = c->source[c->bank + 2][1];
        green = (color >> 5) & 0x1f;
        blue = (color >> 10) & 0x1f;
        red = color & 0x1f;
        scale = c->timer - 0x20;
        red = red * scale >> 5;
        green = green * scale >> 5;
        blue = blue * scale >> 5;
        c->banks[c->bank + 2][1] = red | green << 5 | blue << 10;
    }
    c->timer++;
    if (c->timer > 0x3f)
        c->timer = 0;
}
