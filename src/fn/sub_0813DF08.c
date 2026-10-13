#include "global.h"

void sub_0823233C(void *);
void sub_08013684(void *);
void Effect_Remove(void *);
void sub_0813C720(void *);
void sub_0813C258(void *);

void sub_0813DF08(u8 *p)
{
    sub_0823233C(p + 0x5B8);
    sub_08013684(p + 0x700);
    Effect_Remove(p + 0x748);
    Effect_Remove(p + 0x778);
    Effect_Remove(p + 0x7A0);
    Effect_Remove(p + 0x7D0);
    sub_0813C720(p);
    sub_0813C258(p);
}
