#include "global.h"

struct S {
    u8 f[0x1c];
    u16 a;
};
u32 Actor_SetPaletteBank(struct S *, u16);
u32 Actor_SetDefaultPaletteBank(struct S *p, u16 v)
{
    p->a = v;
    return Actor_SetPaletteBank(p, p->a);
}
