#include "engine/ram.h"
#include "global.h"
#include "link/handshake/slot_monitor.h"

void Serial_StopCommunication(void);

s32 LinkHandshake_OnDestroy(struct SlotMonitor *p)
{
    if (p->f30 == 0) {
        Serial_StopCommunication();
    }
    gSlotMonitor = 0;
    return 0;
}
