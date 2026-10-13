#include "global.h"
#define ResultMenu_Update sub_0801CF30
int sub_0822C220(void);
void sub_0824923C(void *, void *);
void sub_0801C24C(void *);
void sub_08225F88(void *);
int ResultMenu_Update(u8 *p)
{
    *(int *)(p + 0x50) = sub_0822C220();
    sub_0824923C(p, *(void **)(p + 0x7a4));
    sub_0801C24C(p);
    sub_08225F88(p + 0x18);
    return 0;
}
