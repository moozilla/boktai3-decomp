#include "palette_controller.h"

u32 sub_080036DC(void)
{
    struct PaletteController *q = gUnk_02000468;
    u32 r;
    if (q) {
        struct PaletteTransition *t = &q->transition;
        r = t->brightness;
    } else {
        r = 0;
    }
    return r;
}
