#include "global.h"
s32 sub_0822CDA8(s32);
void sub_0822CE18(s32);
s32 sub_0822CF44(s32 a)
{
    s32 i;
    for (i = 0; i <= 0xf; i++) {
        if (sub_0822CDA8(i) == a) {
            sub_0822CE18(i);
            return 1;
        }
    }
    return 0;
}
