#include "global.h"
struct S {
    u8 f[0x1C];
    u32 a;
};
void PaletteTransition_StartDisplay(void *, u32);
void SceneDisplay_InitCurrentAnimation(struct S *p)
{
    PaletteTransition_StartDisplay(p, p->a);
}
