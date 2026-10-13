#include "palette_controller.h"

u16 *sub_08002AC0(struct PaletteController *p, u32 set, u32 slot);
void Palette_ScaleRange(u16 *dest, const u16 *source, u32 brightness, s32 first, s32 end);

void sub_08002E00(struct PaletteController *p, struct PaletteTransition *t)
{
    u16 *r = sub_08002AC0(p, 0, 2);
    Palette_ScaleRange(t->colors, r, 0x32, 0, 0xD0);
}
