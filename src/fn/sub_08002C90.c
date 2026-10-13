#include "palette_controller.h"

void sub_0821AD08(u32, u32);
void sub_08002C90(struct PaletteTransition *t)
{
    u32 z = 0;
    t->brightness = z;
    if (t->callback != 0) {
        sub_0821AD08(t->callback, 0);
        t->callback = z;
    }
}
