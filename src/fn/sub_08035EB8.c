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
void Bg_ClearTilemap(int);
void Wireless_InitPacketQueue(void *);
void Wireless_OnParentNiSessionEvent(u8);
void Wireless_OnParentNiSendComplete(void);
void RfuManager_SetEventCallback(void (*)(u8));
void RfuManager_SetMscCallback(void (*)(void));
void Script_Call(void *, int);
void Sound_FadeOutChannel10(int);
void Wireless_BeginParentNiSession(struct Engine *p)
{
    u8 *scratch = p->scratch;
    int i;
    int bit;
    void *listener;
    Bg_ClearTilemap(0);
    Wireless_InitPacketQueue(p->message);
    Memory_Clear(((u8 *)p) + 0xB8, 12);
    Memory_Clear(((u8 *)p) + 0xC4, 12);
    bit = 8;
    rfu_clearSlot(bit, p->mode);
    RfuManager_SetEventCallback(Wireless_OnParentNiSessionEvent);
    RfuManager_SetMscCallback(Wireless_OnParentNiSendComplete);
    i = 0;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        int mask;
        int active = scratch[2];
        if (scratch) {
            bit = 2;
            mask = bit << i;
        } else {
            bit = 2;
            mask = bit << i;
        }
        if (active & mask) {
            rfu_setRecvBuffer(32, i, ((u8 *)p) + 0xC4, 12);
        }
    } while ((++i) <= 3);
    p->phase = bit;
    setOperation(p, 23, 0);
    listener = *((void **)(((u8 *)p) + 0x4C));
    if (listener) {
        Script_Call(listener, 0);
    }
    Sound_FadeOutChannel10(4);
}
