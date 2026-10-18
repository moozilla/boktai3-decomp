// MATCHING_CONCESSION: register-pinning
#include "global.h"
void sub_082284DC(void);
u32 sub_082489A0(void);
void sub_082284E8(void);
s32 sub_08229440(void)
{
    register u32 shifted asm("r5");
    register u32 status asm("r4");
    sub_082284DC();
    shifted = sub_082489A0() << 24;
    status = shifted >> 24;
    sub_082284E8();
    status &= 15;
    if (status == 1)
        return 1;
    return -1;
}
