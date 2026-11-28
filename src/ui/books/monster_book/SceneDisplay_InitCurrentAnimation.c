#include "global.h"
#include "ui/books/bestiary_display.h"

void PaletteTransition_StartDisplay(struct BestiaryDisplay *, u32);

void SceneDisplay_InitCurrentAnimation(struct BestiaryDisplay *p)
{
    PaletteTransition_StartDisplay(p, p->entry);
}
