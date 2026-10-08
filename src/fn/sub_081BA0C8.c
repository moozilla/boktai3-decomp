#include "global.h"

void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void sub_081B984C(void *, void (*)(void));
void sub_081BA244(void);

void sub_081BA0C8(void *p)
{
    sub_082279A8(3, 5, 4, 4, 4, 0xFFFF, 0xFFFF);
    sub_081B984C(p, sub_081BA244);
}
