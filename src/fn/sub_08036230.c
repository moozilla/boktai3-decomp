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
void Bg_ClearTilemap(int);
void Wireless_InitPacketQueue(void *);
void Wireless_OnChildTransferEvent(u8);
void Wireless_OnChildTransferReceiveRequest(void);
void RfuManager_SetEventCallback(void (*)(u8));
void RfuManager_SetMscCallback(void (*)(void));
void Script_Call(void *, int);
void Wireless_BeginChildTransfer(struct Engine *p)
{
    int index;
    u16 *ids;
    struct RfuLinkStatus **links;
    u8 *buffer;
    void *listener;
    int zero;
    Bg_ClearTilemap(0);
    index = 0;
    {
        int offset = 0x16A;
        u16 *first;
        struct RfuLinkStatus **initial;
        u16 *mine;
        first = (u16 *)(((u8 *)p) + (*(&offset)));
        initial = &gRfuLinkStatus;
        mine = &(*initial)->my.id;
        if (*first != *mine) {
            struct RfuLinkStatus **cursor = initial;
            int offset = 0x16A;
            u16 *next;
            next = (u16 *)((u8 *)p + offset);
            do {
                next++;
                index++;
                if (index > 3) {
                    break;
                }
            } while (*next != (*(struct RfuLinkStatus *volatile *)cursor)->my.id);
        }
    }
    zero = 0;
    p->flag1f = index;
    Wireless_InitPacketQueue(p->message);
    Memory_Clear((u8 *)p + 0xB8, 12);
    buffer = (u8 *)p + 0xC4;
    Memory_Clear(buffer, 12);
    RfuManager_SetEventCallback(Wireless_OnChildTransferEvent);
    RfuManager_SetMscCallback(Wireless_OnChildTransferReceiveRequest);
    rfu_setRecvBuffer(32, p->index, buffer, 12);
    p->phase = 2;
    setOperation(p, 24, zero);
    listener = *(void **)((u8 *)p + 0x4C);
    if (listener) {
        Script_Call(listener, 0);
    }
}
