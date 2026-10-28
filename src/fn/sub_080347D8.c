#include "global.h"
#include "ram.h"
#include "rfu_manager.h"
struct Engine {
    u8 pad0[0x1c];
    u8 flag;
    u8 pad1d[4];
    u8 index;
    u8 pad22[3];
    u8 selected, allowed;
    u8 pad27[0xb];
    u8 pending;
    u8 pad33[5];
    u16 operation;
    u8 pad3a[6];
    u16 timer;
    u8 pad42;
    u8 peers;
    u32 ticks;
    u8 pad48[0xec];
    u8 recv[0x40];
};
struct LinkStatus {
    u8 pad0[2];
    u8 count;
};
extern struct LinkStatus *gRfuLinkStatus;
int RfuManager_SendData(int);
int sub_082238F0(int);
u16 rfu_setRecvBuffer(u8, u8, void *, u32);

static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}

void sub_080347D8(u8 msg)
{
    struct Engine *p = gWirelessController;
    if (p == 0)
        return;
    switch (msg) {
        case 0x50:
            RfuManager_SendData(1);
            break;
        case 0x00:
            setOperation(p, 6, 0);
            break;
        case 0x20:
            p->allowed = gRfuLinkManager.event.param[0];
            break;
        case 0x21:
            setOperation(p, 6, 0);
            break;
        case 0x22:
            p->index = gRfuLinkManager.event.param[0];
            rfu_setRecvBuffer(0x20, p->index, p->recv, 0x40);
            setOperation(p, 0xe, 0);
            break;
        case 0x23:
            setOperation(p, 6, 0);
            break;
        case 0x24: {
            s32 n;
            p->peers = 0;
            n = gRfuLinkStatus->count;
            while (--n != 0)
                p->peers++;
            break;
        }
        case 0x25:
            setOperation(p, 6, 0);
            break;
        case 0x31:
            p->selected |= (u8)gRfuLinkManager.event.param[0];
            p->timer = p->operation;
            setOperation(p, 0x10, 0);
            break;
        case 0x32:
            p->selected &= ~(u8)gRfuLinkManager.event.param[0];
            setOperation(p, p->timer, 0);
            break;
        case 0x30:
        case 0x33:
            p->selected &= ~(u8)gRfuLinkManager.event.param[0];
            if (p->operation > 0xf) {
                setOperation(p, 0x1d, 0);
                sub_082238F0(1);
            } else {
                setOperation(p, 0xb, 0);
            }
            break;
        case 0x40:
            setOperation(p, 0xb, 0);
            p->selected &= ~(u8)gRfuLinkManager.event.param[0];
            break;
        case 0x42:
            setOperation(p, 0x20, 0);
            break;
        case 0x43:
            setOperation(p, 6, 0);
            break;
        case 0x44:
            setOperation(p, 0x1e, 0);
            break;
        case 0x45:
            break;
        case 0xf0:
            if (p->operation == 0x1c)
                break;
            goto stop;
        case 0xf1:
            if (p->operation == 0x1c)
                break;
            goto stop;
        case 0xff:
            if (p->operation == 0x1c || p->operation <= 1)
                break;
        case 0xf2:
        stop:
            sub_082238F0(1);
            setOperation(p, 0x1c, 0);
            break;
    }
}
