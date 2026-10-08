#include "global.h"

void sub_08033468(void);
s32 sub_08033568(void);

void sub_0810E110(u8 *p)
{
    *(u8 *)(p + 0xD05) = 0xFF;
    sub_08033468();
    sub_08033568();
}
