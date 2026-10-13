#include "global.h"
void Script_Call(u32, u32);
void sub_080B309C(u8 *p)
{
    u32 *q = (u32 *)(p + 0x688);
    if (*q != 0) {
        Script_Call(*q, 0);
        *q = 0;
    }
}
