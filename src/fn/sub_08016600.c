#include "global.h"
extern int (*gUnk_08604C58[])(void *);
int PaletteLevels_Update(u8 *p)
{
    int (*f)(void *) = gUnk_08604C58[p[24]];
    if (f)
        f(p);
}
