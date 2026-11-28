#include "effects/bursts/radial_burst.h"

void RadialBurstEffect_BlendSpritePalette(struct RadialBurstFade *);
void CallbackGroup_EmitAtPosition(struct RadialBurst *, struct ContactPosition *);

void PaletteTransition_UpdateFadeIn(struct RadialBurstFade *fade)
{
    if (fade->alpha <= 5)
        fade->flags |= 0x400;
    else
        fade->flags &= ~0x400;
    if (fade->alpha <= 15) {
        fade->alpha++;
        RadialBurstEffect_BlendSpritePalette(fade);
    }
    if (++fade->ticks <= 19)
        CallbackGroup_EmitAtPosition(fade->owner, &fade->position);
}
