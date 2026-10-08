#include "global.h"

void sub_0813EDDC(u8 *, u8 *);
void sub_0813B884(u8 *);

void sub_0813F758(u8 *p, s32 unused, s32 flag)
{
    sub_0813EDDC(p, p + 0x32C);
    sub_0813B884(p);
    p[0x45E] = 0;
}
