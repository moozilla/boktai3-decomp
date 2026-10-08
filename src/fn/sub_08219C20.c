#include "global.h"

struct Heap { u32 a, b, c; };
extern struct Heap gUnk_02000714;

void sub_08219C20(void)
{
    gUnk_02000714.a = 0;
    gUnk_02000714.b = 0;
    gUnk_02000714.c = (0x0201F400 - (u32)&gUnk_02000714) | 0x80000000;
}
