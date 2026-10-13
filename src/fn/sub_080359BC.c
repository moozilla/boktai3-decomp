#include "global.h"

extern u32 gUnk_02000484;
void rfu_REQ_changeMasterSlave(void);
void rfu_waitREQComplete(void);
void rfu_REQ_recvData(void);
void sub_080359BC(u16 v)
{
    if (gUnk_02000484 != 0) {
        if (v == 0x27) {
            rfu_REQ_changeMasterSlave();
            rfu_waitREQComplete();
        } else
            rfu_REQ_recvData();
    }
}
