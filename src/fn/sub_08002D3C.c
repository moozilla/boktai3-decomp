#include "palette_controller.h"

void sub_08002D3C(struct PaletteTransition *t)
{
    if (t->brightness > 1) {
        t->brightness -= 2;
    } else {
        t->brightness = 0;
        t->mode = 0;
    }
}
