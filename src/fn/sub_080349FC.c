#include "global.h"

extern void *gUnk_02000484;
void rfu_REQ_recvData(void);

void sub_080349FC(void)
{
    if (gUnk_02000484)
        rfu_REQ_recvData();
}
