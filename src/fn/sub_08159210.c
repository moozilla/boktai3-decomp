#include "global.h"

extern u8 *gUnk_02000580;
void sub_0813E53C(u8 *);

void sub_08159210(void)
{
    if (gUnk_02000580 != 0) {
        sub_0813E53C(gUnk_02000580);
    }
}
