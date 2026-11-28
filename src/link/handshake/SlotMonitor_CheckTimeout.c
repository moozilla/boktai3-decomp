#include "global.h"
#include "link/handshake/slot_monitor.h"

u8 SlotMonitor_CheckTimeout(struct SlotMonitor *obj)
{
    s32 count;
    s32 i;

    if (obj->checkTimeouts & 1) {
        if (obj->ticks58 > 0) {
            count = 0;
            for (i = 0; i < 4; i++) {
                if (obj->slots[i] != 0xFFFF)
                    count++;
            }
            if (count != 2) {
                if (obj->activeTimer > 90)
                    return 1;
                obj->activeTimer++;
            } else {
                obj->activeTimer = 0;
            }
            obj->idleTimer = 0;
            return 0;
        } else {
            obj->activeTimer = 0;
            if (obj->receiveResult < 0 || obj->receiveResult != obj->result ||
                obj->sendResult < 0) {
                if (obj->idleTimer > 70)
                    return 1;
                obj->idleTimer++;
            } else {
                obj->idleTimer = 0;
            }
        }
    }
    return 0;
}
