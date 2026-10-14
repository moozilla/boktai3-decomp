#include "global.h"

void sub_08058130(void *, u32, u32);
void sub_08058150(void *, void *);
void sub_0805824C(void);

void sub_0805822C(void *p)
{
    sub_08058130(p, 2, 4);
    sub_08058150(p, sub_0805824C);
}
