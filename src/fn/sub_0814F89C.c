#include "global.h"

void sub_0814F838(u8 *);
void sub_0814F874(u8 *);

void sub_0814F89C(u8 *p)
{
    u8 v = p[0x59E];

    if (v == 1) {
        sub_0814F838(p);
    } else if (v == 2) {
        sub_0814F874(p);
    }
}
