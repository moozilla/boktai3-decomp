#include "global.h"
#include "render.h"
#include "ram.h"
// The palette binding at actor offset 0x148 is distinct from the main graphics palette.
s32 Div(s32, s32);
struct PaletteBinding {
    u8 unknown0[0x68];
    u16 *palette;
};
static inline void SetPalette(struct PaletteBinding *binding, u16 *palette)
{
    binding->palette = palette;
}
void SceneNpc_BlendPalette(u8 *actor, s32 step, s32 length)
{
    const u16 *source = (u16 *)(gPaletteBankData + 0x6120);
    const u16 *target = (u16 *)(gPaletteBankData + 0x6140);
    u16 *output = gSceneNpcBlendPalette;
    s32 i;
    s32 inverse;
    s32 tr;
    s32 tg;
    s32 tb;
    u32 mask;
    s32 tg_2;
    u32 k1 = 31;
    SetPalette((struct PaletteBinding *)(actor + 0xe0), output);
    step *= 4;
    length *= 4;
    inverse = length - step;
    tb = 0;
    tg_2 = 0;
    tr = 0;
    mask = k1;
    i = 15;
    do {
        u32 color = *source;
        u32 red = k1;
        u32 green;
        u32 blue;
        red &= color;
        green = ((color << 16) >> 21) & mask;
        blue = ((color << 16) >> 26) & mask;
        if (target) {
            color = *target;
            tr = 31;
            tr &= color;
            tg_2 = ((color << 16) >> 21) & mask;
            tb = ((color << 16) >> 26) & mask;
            target++;
        }
        red = Div((red * inverse) + (tr * step), length) & mask;
        green = Div((green * inverse) + (tg_2 * step), length) & mask;
        blue = Div((blue * inverse) + (tb * step), length) & mask;
        *output = red | ((blue << 10) | (green << 5));
        source++;
        output++;
    } while ((--i) >= 0);
}
