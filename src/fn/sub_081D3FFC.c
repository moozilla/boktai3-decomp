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
void sub_081D4A34(struct S *);

void SceneDisplay_SwitchToAlternateDescriptor(struct S *p)
{
    u32 z;
    BestiaryDisplay_SetEntry(p, SceneDisplay_GetAlternateDescriptor(p));
    z = 0;
    p->f18 = z;
    p->f20 = z;
    p->b24 = z;
    sub_081D4A34(p);
}
