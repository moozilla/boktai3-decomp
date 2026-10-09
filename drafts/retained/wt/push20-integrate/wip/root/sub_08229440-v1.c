#include "global.h"
void sub_082284DC(void);
u8 sub_082489A0(void);
void sub_082284E8(void);

s32 sub_08229440(void)
{
    u32 status;
    sub_082284DC();
    status = (u8)sub_082489A0();
    sub_082284E8();
    if ((status & 15) == 1) return 1;
    return -1;
}
