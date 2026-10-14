#include "global.h"
s32 sub_0822CDD0(s32);
void sub_0822CE00(s32, s32);
/* Add `id` to the first free of 16 slots unless it is already present. */
s32 sub_0822CEAC(s32 id)
{
    s32 i;
    for (i = 0; i <= 15; i++)
        if (sub_0822CDD0(i) == id)
            return 0;
    for (i = 0; i <= 15; i++) {
        if (sub_0822CDD0(i) < 0) {
            sub_0822CE00(i, id);
            return 1;
        }
    }
    return 0;
}
