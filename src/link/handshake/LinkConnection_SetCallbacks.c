#include "global.h"
#include "engine/ram.h"
#include "link/handshake/slot_monitor.h"

void LinkConnection_SetCallbacks(void *context,
                                 void (*onSuccess)(void *context, s32 role, s32 result, u32 mode),
                                 void (*onFailure)(void *context, s32 role, s32 result, u32 mode))
{
    struct SlotMonitor *obj = (struct SlotMonitor *)gSlotMonitor;

    if (obj) {
        obj->context = context;
        obj->onSuccess = onSuccess;
        obj->onFailure = onFailure;
    }
}
