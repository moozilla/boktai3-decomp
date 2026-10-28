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
void WirelessController_RestoreLocalInput(struct Engine *);
void WirelessController_PollInputExchange(struct Engine *);
void Wireless_BuildParentUniPacket(struct Engine *);
void RfuManager_SendData(int);
// MATCHING_CONCESSION: register-pinning
static inline void publishInputs(struct Engine *p)
{
    int enabled = p->flag31;
    if (enabled) {
        register int i asm("r5");
        int valid;
        int mask;
        u16 *output;
        int invalid;
        register u16 *history asm("r4");
        gLinkMenuActive = 1;
        i = 0;
        {
            int initial = 0x2000;
            valid = initial;
        }
        mask = 4095;
        output = gSceneSelectionDisplaySlots;
        {
            register int initial asm("r1") = 65535;
            asm("" : "+r"(initial));
            invalid = initial;
        }
        history = p->history;
        do {
            register int value asm("r2") = *history;
            int test = valid;
            int result;
            if (value & test) {
                result = mask;
                result &= value;
            } else
                result = *output | invalid;
            *output = result;
            output++;
            history++;
        } while (++i <= 4);
    } else
        gLinkMenuActive = enabled;
}
int Wireless_UpdateChildUniSession(struct Engine *p)
{
    u8 *ready = &p->flag31;
    {
        int zero = 0;
        *ready = zero;
        sub_08223BC0(zero);
    }
    rfu_getMasterSlave();
    WirelessController_RestoreLocalInput(p);
    WirelessController_PollInputExchange(p);
    {
        void **handlers = gUnk_086056EC;
        int op = p->operation;
        Callback_InvokeActor(p, handlers[op]);
    }
    publishInputs(p);
}
