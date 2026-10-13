#include "palette_controller.h"

s32 sub_08002C70(void);
void sub_08002CB0(struct PaletteTransition *t)
{
    s32 current;
    current = t->brightness;
    if (current < sub_08002C70()) {
        t->brightness += 4;
        return;
    }
    t->brightness = sub_08002C70();
    t->mode = 2;
}
