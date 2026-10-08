#include "global.h"
s32 sub_081D4E4C(s32);
s32 sub_081D4E5C(s32 n)
{
    s32 s = 0;
    s32 i;
    for (i = 0; i < n; i++) s += sub_081D4E4C(i);
    return s;
}
