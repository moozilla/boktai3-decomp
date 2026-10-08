#include "global.h"

void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_081C5EF0(u8 *, void (*)(void));
void sub_08033468(void);
void sub_080335B4(void);
void sub_081C616C(void);

void sub_081C5F50(u8 *p)
{
    sub_08033468();
    sub_080335B4();
    sub_082279A8(1, 5, 4, 4, 4, 0xFFFF, 0);
    sub_081C5EF0(p, sub_081C616C);
}
