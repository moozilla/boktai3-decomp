#include "global.h"
#include "ui/books/bestiary_display.h"

u32 SceneDisplay_GetAlternateDescriptor(struct BestiaryDisplay *);
void BestiaryDisplay_SetEntry(struct BestiaryDisplay *, u32);
void PaletteTransition_Restart(struct BestiaryDisplay *);

void SceneDisplay_SwitchToAlternateDescriptor(struct BestiaryDisplay *p)
{
    BestiaryDisplay_SetEntry(p, SceneDisplay_GetAlternateDescriptor(p));
    p->swayPhase = 0;
    p->frame = 0;
    p->state = 0;
    PaletteTransition_Restart(p);
}
