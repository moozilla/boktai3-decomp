#include "ram.h"
#include "global.h"

void Serial_StopCommunication(void);

struct SlotMonitor {
    u8 unk0[0x18];
    s32 timeout;
    u32 param, f20;
    s32 result;
    u32 f28, f2c, f30, f34, f38;
    u32 f3c, f40;
    s32 f44, f48;
    u8 unk4c[0x10];
    u16 idleTimer, activeTimer, flags, slots[4], unk6a, config;
    u8 unk6e, mode;
    u32 f70, f74, f78;
    void (*callback)(void);
    u32 f80;
};

s32 LinkHandshake_OnDestroy(struct SlotMonitor *p)
{
    if (p->f30 == 0) {
        Serial_StopCommunication();
    }
    gSlotMonitor = 0;
    return 0;
}
