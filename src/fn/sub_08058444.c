#include "global.h"

void sub_08058130(void *, s32, s32);
void sub_08058150(void *, void (*)(void));
void sub_0805849C(void);

void sub_08058444(void *p)
{
    sub_08058130(p, 2, 12);
    sub_08058150(p, sub_0805849C);
}
