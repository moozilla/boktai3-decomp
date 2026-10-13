#include "palette_controller.h"

u32 sub_08002AB8(struct PaletteController *p, u32 x)
{
    return p->pending & x;
}
