#include "global.h"

void sub_08219DD8(u32, u32);

void Clock_ClearState(void)
{
    sub_08219DD8(0x03005430, 0x24);
}
