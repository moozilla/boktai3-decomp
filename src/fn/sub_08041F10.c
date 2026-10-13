#include "global.h"

void sub_080414A0(u8 *);
void sub_082156B8(u32);
void sub_08040FE4(u8 *);

void sub_08041F10(u8 *p)
{
    sub_080414A0(p);
    sub_082156B8(0);
    sub_082156B8(1);
    sub_082156B8(2);
    sub_082156B8(3);
    sub_08040FE4(p);
}
