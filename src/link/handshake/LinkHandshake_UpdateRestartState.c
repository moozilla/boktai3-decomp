#include "global.h"
#include "link/handshake/slot_monitor.h"

void SerialLink_ResetMultiplayer(void);

void LinkHandshake_UpdateRestartState(struct SlotMonitor *obj)
{
    if (obj->restartPending) {
        SerialLink_ResetMultiplayer();
        obj->restartPending = FALSE;
    } else {
        obj->callback = SlotMonitor_PollResult;
        obj->retryTimer = 0;
        obj->errorFrames = 0;
        obj->roleRetryTimer = 0;
        obj->ticks58 = 0;
    }
}
