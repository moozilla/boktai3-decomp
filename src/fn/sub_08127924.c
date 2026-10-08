#include "global.h"
extern u8 *gUnk_02000164;
u32 sub_08127924(u8 *p)
{
    *(u32 *)(p + 0x860) = 0;
    gUnk_02000164 = p;
    return 0;
}
