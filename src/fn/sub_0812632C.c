#include "global.h"
void sub_08214514(u8 *);
void sub_0812632C(u8 *p)
{
    sub_08214514(p);
    p[0x6c] = 0xff;
    p[0x58] = 0;
}
