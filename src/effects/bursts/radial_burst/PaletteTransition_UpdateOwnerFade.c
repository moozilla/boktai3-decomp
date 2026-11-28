#include "effects/bursts/radial_burst.h"
#include "player/player.h"

void PaletteTransition_UpdateFadeIn(struct RadialBurstFade *);
void PaletteTransition_UpdateFadeOut(struct RadialBurstFade *);

void PaletteTransition_UpdateOwnerFade(struct RadialBurst *burst)
{
    if (burst->fade.active) {
        if (burst->player->tetherMode == 9 && burst->player->tetherPhase <= 2 &&
            !burst->fade.fadingOut) {
            PaletteTransition_UpdateFadeIn(&burst->fade);
        } else {
            burst->fade.fadingOut = 1;
            PaletteTransition_UpdateFadeOut(&burst->fade);
        }
    }
}
