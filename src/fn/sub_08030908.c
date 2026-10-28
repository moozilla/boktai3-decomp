#include "global.h"
#include "render.h"

struct S {
    u8 f[0x1ec];
    u8 *a;
};
void MessageWindow_ApplyPalette(struct S *p)
{
    CpuSet(p->a + 0x1f4, gBgPaletteBank15, 0x04000008);
}
