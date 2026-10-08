#include "global.h"

void sub_08002D58(void *, u8 *);

void sub_08002E28(void *a, u8 *p)
{
    sub_08002D58(a, p);
    (*p)++;
    if (*p > 0x3f)
        *p = 0x40;
}
