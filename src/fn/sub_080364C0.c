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
void sub_08223BC0(int);
void WirelessController_UpdatePeerRosterSend(struct Engine *);
void WirelessController_PollPeerRosterSend(struct Engine *);
int Callback_InvokeActor(void *, void *);
extern void *gUnk_086056EC[];
int Wireless_UpdateConnectionSession(struct Engine *argument)
{
    struct Engine *p = argument;
    if (p->pad18) {
        volatile u8 *busy = &gWirelessSessionState;
        int initial = *busy;
        u8 *lost = &p->flag35;
        if (initial == 1) {
            do {
            } while (*busy == 1);
        }
        sub_08223BC0(0);
        if (*lost) {
            rfu_REQ_disconnect(*lost);
            rfu_waitREQComplete();
            *lost = 0;
        }
        {
            int mode = gUnk_03006A80->parentChild;
            if (mode == 1) {
                gWirelessSessionState = mode;
                rfu_REQ_sendData(1);
            }
        }
    }
    WirelessController_UpdatePeerRosterSend(p);
    WirelessController_PollPeerRosterSend(p);
    Callback_InvokeActor(p, gUnk_086056EC[p->operation]);
}
