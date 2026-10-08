#include "global.h"

void sub_08058130(void *, s32, s32);
void sub_08058150(void *, void (*)(void));
void sub_080581C0(void);

void sub_080581A0(void *p)
{
    sub_08058130(p, 2, 3);
    sub_08058150(p, sub_080581C0);
}
