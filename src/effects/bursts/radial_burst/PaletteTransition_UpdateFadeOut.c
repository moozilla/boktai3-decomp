#include "effects/bursts/radial_burst.h"

void RadialBurstEffect_BlendSpritePalette(struct RadialBurstFade *);

void PaletteTransition_UpdateFadeOut(struct RadialBurstFade *fade)
{
    if (fade->alpha <= 5)
        fade->flags |= 0x400;
    else
        fade->flags &= ~0x400;
    if (fade->alpha == 0) {
        fade->flags |= 1;
        fade->active = 0;
    } else {
        fade->alpha--;
        RadialBurstEffect_BlendSpritePalette(fade);
    }
}
