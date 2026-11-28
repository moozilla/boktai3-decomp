#include "global.h"
#include "link/handshake/slot_monitor.h"

void LinkConnection_SetRetryOptions(struct SlotMonitor *obj, u32 options)
{
    obj->checkTimeouts = FALSE;
    if (options & 1)
        obj->checkTimeouts = TRUE;
    if (options & 4)
        obj->limit = 30;
    else
        obj->limit = 120;
}
