#include "global.h"

struct RfuStatic {
    u8 pad0[2];
    u8 f2;
    vu8 f3;
    u8 pad4[0xa];
    u8 fE;
    u8 fF;
    u8 pad10[0x34];
    u32 f44;
};
extern struct RfuStatic gUnk_03005390;
extern u8 *gUnk_03006A80;
void RfuManager_UpdateLinkRecovery(u16);
void RfuManager_CompletePendingMscCycle(void);
u16 rfu_UNI_PARENT_getDRAC_ACK(u8 *);
void Callback_InvokeActor(u16, u32);
void rfu_waitREQComplete(void);

void Rfu_ReqCallback(u16 a)
{
    u8 saved = gUnk_03005390.fE;
    u8 zero, ack;
    u32 cb;
    gUnk_03005390.fE = 0;
    gUnk_03005390.fF = 1;
    zero = *gUnk_03006A80;
    if (zero == 0) {
        RfuManager_UpdateLinkRecovery(a);
        if (gUnk_03005390.f2 != 1) {
            RfuManager_CompletePendingMscCycle();
            gUnk_03005390.fF = zero;
            gUnk_03005390.fE = saved;
            return;
        }
    } else {
        if (rfu_UNI_PARENT_getDRAC_ACK(&ack) == 0) {
            gUnk_03005390.f3 |= ack;
        }
    }
    cb = gUnk_03005390.f44;
    if (cb != 0) {
        Callback_InvokeActor(a, cb);
        rfu_waitREQComplete();
        if (gUnk_03005390.f2 == 2) {
            RfuManager_CompletePendingMscCycle();
        }
    }
    gUnk_03005390.fF = 0;
    gUnk_03005390.fE = saved;
}
