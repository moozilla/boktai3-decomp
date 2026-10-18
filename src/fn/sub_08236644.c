#include "global.h"
#include "player.h"
struct OwnerPaletteTransition {
    u8 unknown0[0x1c];
    struct Player *owner;
    u8 unknown20[0xa0];
    u8 fade[0x6c];
    u8 active;
    u8 unknown12D;
    u8 fadingOut;
};
void PaletteTransition_UpdateFadeIn(void *);
void PaletteTransition_UpdateFadeOut(void *);
void PaletteTransition_UpdateOwnerFade(struct OwnerPaletteTransition *transition)
{
    if (transition->active) {
        if (transition->owner->tetherMode == 9 && transition->owner->tetherPhase <= 2 &&
            !transition->fadingOut)
            PaletteTransition_UpdateFadeIn(transition->fade);
        else {
            transition->fadingOut = 1;
            PaletteTransition_UpdateFadeOut(transition->fade);
        }
    }
}
