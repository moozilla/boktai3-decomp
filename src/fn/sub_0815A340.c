#include "global.h"

extern u8 *gUnk_02000580;

void sub_0815A340(void)
{
    u8 *g = gUnk_02000580;

    if (g != 0) {
        g[0x740] = 1;
    }
}
