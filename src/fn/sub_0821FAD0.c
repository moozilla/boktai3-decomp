#include "global.h"
struct A {
    u8 p[0x14];
    void *f14;
};
extern struct A *gTerrainMap;
void sub_0821F864(void *, u16 *, u32);
void NavMesh_StepTowardTarget(void *, u16 *, u32);
void NavMesh_UpdateProbe(u16 *a, u32 b)
{
    void *t = gTerrainMap->f14;
    sub_0821F864(t, a, b);
    if (*a & 1)
        NavMesh_StepTowardTarget(t, a, b);
}
