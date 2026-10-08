#include "global.h"

s32 sub_08228DB8(void);

s32 sub_081EA78C(void)
{
    s32 r;
    switch (sub_08228DB8()) {
    case 1:
    case 2:
    case 3:
        r = 0;
        break;
    case 0:
    case 4:
    case 5:
        r = 1;
        break;
    default:
        r = -1;
        break;
    }
    return r;
}
