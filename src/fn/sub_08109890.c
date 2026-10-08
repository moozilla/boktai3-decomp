#include "global.h"

void sub_08109584(u8 *);

void sub_08109890(u8 *p)
{
    if (p[0x35] == 1) {
        sub_08109584(p);
        p[0x35] = 0;
    }
}
