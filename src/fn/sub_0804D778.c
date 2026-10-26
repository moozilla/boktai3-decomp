#include "staged_effect.h"
void StagedEffect_BlendPalette(struct StagedEffect *effect, s32 first, s32 second, s32 blend)
{
    const u16 *a = gPaletteBankData + (first + 0x283) * 16;
    const u16 *b = gPaletteBankData + (second + 0x283) * 16;
    u16 *out;
    s32 inverse;
    s32 i;
    if (blend == 0) {
        effect->activePalette = a;
    } else if (blend == 64) {
        effect->activePalette = b;
    } else {
        out = effect->blendedPalette;
        inverse = 64 - blend;
        i = 15;
        do {
            s32 redA = *a & 0x1F;
            s32 greenA = *a & 0x3E0;
            s32 blueA = *a & 0x7C00;
            s32 redB = *b & 0x1F;
            s32 greenB = *b & 0x3E0;
            s32 blueB = *b & 0x7C00;
            redA = (inverse * redA + blend * redB) >> 6;
            greenA = (inverse * greenA + greenB * blend) >> 6;
            blueA = (inverse * blueA + blueB * blend) >> 6;
            redA &= 0x1F;
            greenA &= 0x3E0;
            blueA &= 0x7C00;
            *out = redA | greenA | blueA;
            a++;
            b++;
            out++;
        } while (--i >= 0);
        effect->activePalette = effect->blendedPalette;
    }
}
