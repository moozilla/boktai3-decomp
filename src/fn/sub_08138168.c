#include "global.h"
u32 sub_08138168(u8 *p)
{
    if (p[0x22]) {
        p[0x22] = 0;
        return 1;
    }
    return 0;
}
