#include "palette_controller.h"

void sub_08002AA8(struct PaletteController *p, u32 x)
{
    p->pending |= x;
}
