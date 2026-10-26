// MATCHING_CONCESSION: register-pinning
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
extern struct Engine *gUnk_02000484;
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
void WirelessController_RebuildAvailablePeerList(struct Engine *);
int RfuManager_StartChildConnect(int, int);
extern u16 gUnk_03005260[];
int Wireless_SelectParentFromInput(struct Engine *p)
{
    u8 *count;
    if (p->pending)
        p->pending = 0;
    WirelessController_RebuildAvailablePeerList(p);
    {
        u8 *initial = &p->count;
        int available = *initial;
        {
            u8 *savedCount = initial;
            count = savedCount;
        }
        if (available) {
            s8 *scroll = &p->scroll;
            int old = *scroll;
            register u16 *buttons asm("r2") = gUnk_03005260;
            register int pressed asm("r5");
            int current;
            int index;
            int n;
            pressed = buttons[1];
            if (pressed & 64)
                (*scroll)--;
            else if (pressed & 128)
                (*scroll)++;
            current = *scroll;
            if (old != current) {
                if (current > 7)
                    *scroll = 7;
                else if (current < 0)
                    *scroll = 0;
            }
            index = (s8)(u8)*scroll;
            n = *count;
            if ((buttons[1] & 1) && index < n) {
                int parentIndex = p->ids[index];
                struct RfuLinkStatus *link;
                int status;
                link = gUnk_03006A80;
                status = (u8)RfuManager_StartChildConnect(link->partner[parentIndex].id, 0);
                if (!status) {
                    setOperation(p, 10, status);
                    return 0;
                }
            }
        }
    }
}
