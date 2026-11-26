#include "objects/warp_circle/warp_circle.h"
void AnimData_SelectPaletteBank(void *, u32);
void WarpCircle_TickDisabled(struct WarpCircle *p)
{
    p->sprite.flags |= 1;
    AnimData_SelectPaletteBank(p->graphics, p->paletteBase);
}
