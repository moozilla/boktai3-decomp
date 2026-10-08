#include "global.h"
void sub_08115398(u8 *p)
{
    if (p[0xf1]) {
        p[0xf0] = 0;
        p[0xf1] = 0;
    }
}
