#include "global.h"

struct S {
    u8 f[7];
    u8 v;
};
void Storage_RebuildGrid(struct S *);

void Storage_SetGridFlags(struct S *p, u8 v)
{
    p->v = v;
    Storage_RebuildGrid(p);
}
