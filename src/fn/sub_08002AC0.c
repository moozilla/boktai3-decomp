#include "palette_controller.h"

u16 *sub_08002AC0(struct PaletteController *p, u32 set, u32 slot)
{
    return p->colorSets[set][slot];
}
