#include "global.h"
#ifdef NONMATCHING
// fuzzy: 70.6%
// 6 differing instruction runs; first diff at +0x0 (push {r4, r5, lr} vs push {r4, lr})
void sub_082284DC(void);
u8 sub_082489A0(void);
void sub_082284E8(void);

s32 sub_08229440(void)
{
    u8 status;
    sub_082284DC();
    status = sub_082489A0();
    sub_082284E8();
    if ((status & 15) == 1)
        return 1;
    return -1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08229440);
#endif
