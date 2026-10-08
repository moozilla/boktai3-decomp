#include "global.h"

void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void sub_0821447C(u32, void (*)(void), void (*)(void), void (*)(void));
u32 sub_08039548(void)
{
    sub_0821447C(0, sub_08217F4C, sub_08214B44, sub_08219624);
    return 0;
}
