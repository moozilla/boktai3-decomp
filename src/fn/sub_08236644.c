#define PaletteTransition_UpdateOwnerFade sub_08236644
#define PaletteTransition_UpdateFadeIn    sub_0823659C
#define PaletteTransition_UpdateFadeOut   sub_082365F4
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
void sub_0823659C(void *);
void sub_082365F4(void *);
void sub_08236644(struct OwnerPaletteTransition *transition)
{
    if (transition->active) {
        if (transition->owner->tetherMode == 9 && transition->owner->tetherPhase <= 2 &&
            !transition->fadingOut)
            sub_0823659C(transition->fade);
        else {
            transition->fadingOut = 1;
            sub_082365F4(transition->fade);
        }
    }
}
