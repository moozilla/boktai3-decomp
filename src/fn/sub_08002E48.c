#include "palette_controller.h"

void sub_08002D90(struct PaletteController *, struct PaletteTransition *);

void sub_08002E48(struct PaletteController *p, struct PaletteTransition *t)
{
    sub_08002D90(p, t);
}
