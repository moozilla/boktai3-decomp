#include "palette_controller.h"

void sub_08003448(u32 v)
{
    if (gUnk_02000468) {
        struct PaletteTransition *t = &gUnk_02000468->transition;
        t->mode = v;
    }
}
