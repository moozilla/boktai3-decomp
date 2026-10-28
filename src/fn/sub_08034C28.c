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
int RfuManager_StartConnect(int, int, int, const u8 *);
extern const u8 gUnk_0824DE60[];
int Wireless_StartChildSearch(struct Engine *p)
{
    int raw;
    int status;
    if (p->pending) {
        int i;
        void *buffer;
        p->pending = 0;
        i = 0;
        buffer = p->work;
        do {
            rfu_setRecvBuffer(32, (u8)i, buffer, 4);
            i++;
        } while (i <= 3);
    }
    raw = RfuManager_StartConnect(1, 0, 600, gUnk_0824DE60);
    {
        int narrowed = raw;
        status = (u8)narrowed;
    }
    if (!status) {
        goto success;
    }
    setOperation(p, 27, 0);
    return -1;

success: {
    int operation = 7;
    int one;
    p->flag = status;
    p->operation = operation;
    p->ticks = status;
    {
        u8 *pending = &p->pending;
        one = 1;
        *pending = one;
    }
}
}
