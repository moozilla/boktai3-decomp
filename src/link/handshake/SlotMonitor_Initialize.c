#include "global.h"
#include "engine/ram.h"
#include "link/handshake/slot_monitor.h"
void Solar_PauseMeasurement(void);
void Serial_InitMultiplayer(void);
void sub_08223180(void);
void LinkConnection_SetRetryOptions(void *, u32);
s32 SlotMonitor_Initialize(struct SlotMonitor *obj, s32 timeout, u32 param, u32 setting, s32 config,
                           u32 mode)
{
    gSlotMonitor = obj;
    Solar_PauseMeasurement();
    gClockSynced = 1;
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
    obj->finished = 0;
    obj->f30 = 0;
    obj->elapsed = 0;
    obj->readyFrames = 0;
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
