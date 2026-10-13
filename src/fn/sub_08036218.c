#include "global.h"

extern u32 gUnk_02000484;
void rfu_REQ_recvData(void);
void sub_08036218(void)
{
    if (gUnk_02000484)
        rfu_REQ_recvData();
}
