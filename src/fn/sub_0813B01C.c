#include "global.h"
u32 sub_0813B01C(u8 *p)
{
    if (p[0x45c]) {
        p[0x45c] = 0;
        return 1;
    }
    return 0;
}
