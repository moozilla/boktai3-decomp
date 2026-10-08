#include "global.h"
s32 sub_08049818(u32 a, u32 b, u32 c, u32 d)
{
    s32 r;
    if (b & 4) {
        if (d & 4) {
            if (a == c) goto yes;
        }
    no:
        r = 0;
        goto done;
    } else {
        if (d & 4) goto no;
    }
yes:
    r = 1;
done:
    return r;
}
