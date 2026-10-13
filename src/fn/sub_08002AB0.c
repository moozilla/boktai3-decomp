#include "palette_controller.h"

void sub_08002AB0(struct PaletteController *p, u32 x)
{
    p->pending &= ~x;
}
