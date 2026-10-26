#include "global.h"
#include "ram.h"
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
extern u32 gUnk_0300546C;
void Solar_PauseMeasurement(void);
void Serial_InitMultiplayer(void);
void sub_08223180(void);
void LinkConnection_SetRetryOptions(void *, u32);
void SlotMonitor_PollResult(void);
s32 SlotMonitor_Initialize(struct SlotMonitor *obj, s32 timeout, u32 param, u32 setting, s32 config,
                           u32 mode)
{
    gSlotMonitor = obj;
    Solar_PauseMeasurement();
    gUnk_0300546C = 1;
    Serial_InitMultiplayer();
    sub_08223180();
    if (timeout == 0)
        obj->timeout = 300;
    else
        obj->timeout = timeout;
    LinkConnection_SetRetryOptions(obj, setting);
    obj->config = (config + (config >> 8)) & 0x3f;
    obj->mode = mode & 0x3f;
    obj->param = param;
    obj->f20 = 0;
    obj->result = -1;
    obj->f28 = 0;
    obj->f2c = 0;
    obj->f30 = 0;
    obj->f34 = 0;
    obj->f38 = 0;
    obj->activeTimer = 0;
    obj->idleTimer = 0;
    obj->f44 = -1;
    obj->f48 = -1;
    obj->flags = 0;
    obj->slots[0] = 0xffff;
    obj->slots[1] = -1;
    obj->slots[2] = -1;
    obj->slots[3] = -1;
    obj->f80 = 0;
    obj->f74 = 0;
    obj->f78 = 0;
    obj->callback = SlotMonitor_PollResult;
    return 0;
}
