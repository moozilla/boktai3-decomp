#include "global.h"
struct P {
    u32 a[8];
};
struct S {
    u8 f[0x18];
    struct P p;
    struct P *q;
};
struct P *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void sub_08122D20(struct S *s)
{
    s->q = ScriptProc_Start(0xcb05, 0x1bc7);
    s->p = *s->q;
    ResourceHeader_Relocate(&s->p, s->q);
}
