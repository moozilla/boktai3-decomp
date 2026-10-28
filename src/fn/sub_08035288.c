#include "global.h"
struct Engine {
    u8 pad0[0x18];
    u8 pad18, bits, pad1a, entered, flag, pad1d, state, flag1f, mode, index;
    u16 mask;
    s8 scroll;
    u8 selected, allowed, count, ids[8], active, flag31, pending, flag33, phase, flag35, flag36,
        pad37;
    u16 operation, value;
    u8 flag3c, flag3d, flag3e, flag3f;
    u16 timer;
    u8 flag42, flag43;
    u32 ticks;
    void *pointer;
    u8 pad4c[0x58];
    u32 number;
    u8 work[16];
    u8 padb8[0x34];
    u16 flags, backup, history[5];
    u8 changed, padfb[5];
    u16 diff[5];
    u8 pad10a[2];
    u32 key;
    u16 input[5];
    u8 pad11a[0x1A];
    u8 scratch[64];
    u8 message[0x204];
    void *head, *tail;
};
void Memory_Clear(void *, int);
#define gRfuLinkStatus   gUnk_03006A80
#define gRfuSlotStatusNI gUnk_03006A70
#include "rfu_compat.h"
#include "ram.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
int WirelessController_ConsumeAdvanceRequest(struct Engine *);
void WirelessController_ResetSession(struct Engine *);
void Wireless_SetStatus(int);
int sub_082238F0(int);
int Wireless_HandleSendError(struct Engine *p)
{
    u8 *pending = &p->pending;
    if (*pending) {
        int zero = 0;
        *pending = zero;
        Wireless_SetStatus(2);
        sub_082238F0(1);
        p->entered = zero;
        p->flag = zero;
    }
    if (WirelessController_ConsumeAdvanceRequest(p)) {
        int operation = 30;
        int zero = 0;
        p->flag = zero;
        p->operation = operation;
        p->ticks = zero;
        *pending = 1;
    }
    return 0;
}