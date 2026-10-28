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
#include "rfu_compat.h"
#include "ram.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
void sub_08223BC0(int);
void RfuManager_SendData(int);
void Wireless_TryStartParentNiSend(struct Engine *);
void Wireless_PollParentNiSend(struct Engine *);
int Callback_InvokeActor(void *, void *);
extern void *gUnk_086056EC[];
void Wireless_UpdateParentNiSession(struct Engine *argument)
{
    struct Engine *p = argument;
    volatile u8 *busy = &gWirelessSessionState;
    u8 *lost;
    while (*busy == 1)
        ;
    sub_08223BC0(0);
    lost = &p->flag35;
    if (*lost) {
        rfu_REQ_disconnect(*lost);
        rfu_waitREQComplete();
        *lost = 0;
    }
    {
        int mode = gRfuLinkStatus->parentChild;
        if (mode == 1) {
            gWirelessSessionState = mode;
            RfuManager_SendData(1);
        }
    }
    Wireless_TryStartParentNiSend(p);
    Wireless_PollParentNiSend(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}
