#include "global.h"
s32 *Script_ReadProcTable(s32 *p, s32 *n)
{
    s32 c;
    s32 *q;
    s32 *o = n;
    c = 0;
    q = p;
    while (*q != -1) { q++; c++; }
    *o = c;
    return q + 1;
}
