#include "global.h"

struct S {
    u8 pad[0x18];
    u32 f18;
    u32 pad1c;
    u32 f20;
    u8 b24;
};
u32 SceneDisplay_GetAlternateDescriptor(struct S *);
void BestiaryDisplay_SetEntry(struct S *, u32);
void PaletteTransition_Restart(struct S *);

void SceneDisplay_SwitchToAlternateDescriptor(struct S *p)
{
    BestiaryDisplay_SetEntry(p, SceneDisplay_GetAlternateDescriptor(p));
    p->f18 = 0;
    p->f20 = 0;
    p->b24 = 0;
    PaletteTransition_Restart(p);
}
