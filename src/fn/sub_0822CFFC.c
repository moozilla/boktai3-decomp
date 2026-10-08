#include "global.h"
s32 sub_0822CDA8(s32);
s32 sub_0822CFFC(s32 a)
{
    s32 i;
    for (i = 0; i <= 0xf; i++) {
        if (sub_0822CDA8(i) == a)
            return 1;
    }
    return 0;
}
