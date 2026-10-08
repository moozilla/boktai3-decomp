#include "global.h"

extern u8 *gUnk_02000484;

void sub_08036C38(void)
{
    if (gUnk_02000484) {
        gUnk_02000484[0x3D] = 1;
    }
}
