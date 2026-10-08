#include "global.h"

void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void sub_081BA62C(void *, void (*)(void));
void sub_081BAA6C(void);

void sub_081BAAE8(void *p)
{
    sub_082279A8(3, 5, 4, 4, 4, 0xFFFF, 0);
    sub_081BA62C(p, sub_081BAA6C);
}
