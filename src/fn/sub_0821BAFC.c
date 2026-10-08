#include "global.h"
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);
void sub_0821A284(u32, void *, u32);
void sub_0821BB34(void);
extern void *gUnk_030052F4;
void sub_0821BAFC(void)
{
    void *p = sub_08219C40(0xE24);
    sub_08219DD8(p, 0xE24);
    sub_0821A284(0x56C2, p, 1);
    gUnk_030052F4 = p;
    sub_0821BB34();
}
