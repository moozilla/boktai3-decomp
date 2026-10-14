#include "global.h"
s32 sub_0822CDD0(s32);
void sub_0822D0C8(s32, s32);
/* Close the gaps in the 16 slots from `start`, then sort the used slots. */
void sub_0822D188(s32 start)
{
    s32 used = 0;
    s32 gap = 0;
    s32 i, j;
    for (i = start; i <= 15; i++) {
        if (sub_0822CDD0(i) < 0)
            gap++;
        else {
            used++;
            if (gap > 0)
                sub_0822D0C8(i, i - gap);
        }
    }
    if (used > 1) {
        for (i = 0; i < used - 1; i++) {
            for (j = i; j < used; j++) {
                if (sub_0822CDD0(i) > sub_0822CDD0(j))
                    sub_0822D0C8(i, j);
            }
        }
    }
}
