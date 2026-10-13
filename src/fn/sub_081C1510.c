#include "global.h"
struct Q {
    u32 a[8];
};
struct S {
    u8 f[0x18];
    struct Q q1;
    struct Q q2;
};
struct Q *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void sub_081C1434(void *);
void sub_081C1510(struct S *p)
{
    struct Q *r;
    u32 id = 0xCB05;
    r = ScriptProc_Start(id, 0xE2AB);
    p->q1 = *r;
    ResourceHeader_Relocate(&p->q1, r);
    r = ScriptProc_Start(id, 0x530D);
    p->q2 = *r;
    ResourceHeader_Relocate(&p->q2, r);
    sub_081C1434(p);
}
