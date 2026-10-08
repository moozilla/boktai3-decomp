#include "global.h"
void sub_08072D34(void);
void sub_08073B38(void);
u32 sub_08073B60(u8 *p)
{
    sub_08072D34();
    sub_08073B38();
    *(u32 *)(p + 0x20) += 1;
}
