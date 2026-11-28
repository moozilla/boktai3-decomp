#include "global.h"
#include "link/handshake/slot_monitor.h"

u8 LinkHandshake_AdvanceTimeout(struct SlotMonitor *obj)
{
    if (obj->timeout < 0)
        return FALSE;
    if (++obj->elapsed >= obj->timeout)
        return TRUE;
    return FALSE;
}
