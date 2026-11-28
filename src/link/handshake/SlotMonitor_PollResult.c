#include "global.h"
#include "link/handshake/slot_monitor.h"

s32 Serial_DetectRoleAndEnableInterrupt(void);
void SerialLink_ResetMultiplayer(void);

void SlotMonitor_PollResult(struct SlotMonitor *obj)
{
    obj->ticks++;
    obj->result = Serial_DetectRoleAndEnableInterrupt();
    if (obj->result < 0) {
        if (obj->roleRetryTimer > 29) {
            SerialLink_ResetMultiplayer();
            obj->result = Serial_DetectRoleAndEnableInterrupt();
            obj->roleRetryTimer = 0;
        } else {
            obj->roleRetryTimer++;
        }
    } else {
        obj->callback = sub_08108C94;
        obj->flags = 0x1BC0 | obj->config;
        obj->errorFrames = 0;
        obj->readyFrames = 0;
        obj->roleRetryTimer = 0;
        return;
    }
    if (LinkHandshake_AdvanceTimeout(obj))
        obj->finished = 1;
    obj->errorFrames = 0;
    obj->readyFrames = 0;
}
