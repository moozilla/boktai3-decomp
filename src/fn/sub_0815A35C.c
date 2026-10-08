#include "global.h"

struct O0815A35C { u8 f0[0xD4]; u32 fD4; };
extern struct O0815A35C *gUnk_02000580;
extern u8 *gUnk_030042E4;

u32 sub_0815A35C(void)
{
    if (gUnk_02000580 == 0)
        return (u32)(gUnk_030042E4 + 0x540);
    return gUnk_02000580->fD4;
}
