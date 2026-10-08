#include "global.h"

struct B { u8 f[0x21]; u8 v; };
void sub_08221110(void *, void *, void *, u32, u32);

void sub_0800E10C(struct B *a, u8 *b)
{
    sub_08221110(b + 4, b + 0x44, b + 0x24, *((u8 *)a + 0x21), 6);
}
