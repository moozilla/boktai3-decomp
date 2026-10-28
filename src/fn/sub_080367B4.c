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
    u32 frameCounter;
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
#include "runtime.h"
static inline void setOperation(struct Engine *p, int op, int zero)
{
    p->flag = zero;
    p->operation = op;
    p->ticks = zero;
    p->pending = 1;
}
void Solar_PauseMeasurement(void);
void Solar_ResumeMeasurement(void);
void Task_UnlinkFromPriorityList(void *);
void Heap_Free(void *);
int sub_082238F0(int);
void Wireless_UpdateConnectionSession(struct Engine *);
void Wireless_UpdateInactiveConnectionSession(struct Engine *);
void Wireless_UpdateParentUniSession(struct Engine *);
void Wireless_UpdateChildUniSession(struct Engine *);
void Wireless_UpdateParentNiSession(struct Engine *);
void Wireless_UpdateChildNiSession(struct Engine *);
int Wireless_UpdateControllerTask(struct Engine *p)
{
    Solar_PauseMeasurement();
    gWirelessLastStatus = p->flag33;
    switch (p->phase) {
        case 0:
            if (p->active)
                Wireless_UpdateConnectionSession(p);
            else
                Wireless_UpdateInactiveConnectionSession(p);
            break;
        case 1:
            if (p->active)
                Wireless_UpdateParentUniSession(p);
            else
                Wireless_UpdateChildUniSession(p);
            break;
        case 2:
            if (p->active)
                Wireless_UpdateParentNiSession(p);
            else
                Wireless_UpdateChildNiSession(p);
            break;
    }
    if (p->flag3d) {
        int zero = 0;
        p->flag3d = zero;
        sub_082238F0(1);
        setOperation(p, zero, zero);
    }
    if (p->pad1d) {
        gClockSynced = 0;
        gWirelessController = 0;
        Task_UnlinkFromPriorityList(p);
        Heap_Free(p);
        Solar_ResumeMeasurement();
    } else {
        if ((u16)(p->operation - 9) <= 17)
            gSceneDialogActive = 1;
        p->ticks++;
        p->frameCounter++;
    }
    return 0;
}
