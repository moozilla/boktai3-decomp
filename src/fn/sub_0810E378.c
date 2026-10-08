#include "global.h"

void sub_0810E218(u8 *);
void sub_0810E1DC(u8 *);

void sub_0810E378(u8 *p)
{
    u8 *flag = p + 0xD2C;

    if (*flag) {
        sub_0810E218(p);
        sub_0810E1DC(p);
        *flag = 0;
    }
}
