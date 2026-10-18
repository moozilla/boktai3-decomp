#include "global.h"
#define SlotMonitor_CheckTimeout sub_08108ADC
struct SlotMonitor {
    u8 unk0[0x24]; s32 expected; u8 unk28[0x1c]; s32 state44; s32 current;
    u8 unk4c[0xc]; s32 mode; u16 idleTimer; u16 activeTimer; u16 unk60;
    u16 slots[4]; u8 unk6a[6]; u32 flags;
};
s32 sub_08108ADC(struct SlotMonitor *obj)
{
    s32 count, i;
    u16 *slot;
    if (obj->flags & 1) {
        if (obj->mode > 0) {
            u16 *timer;
            u16 invalid;
            count = 0;
            timer = &obj->activeTimer;
            invalid = 0xffff;
            slot = obj->slots;
            for (i = 3; i >= 0; i--) {
                if (*slot != invalid) count++;
                slot++;
            }
            if (count != 2) {
                if (*timer > 90) return 1;
                (*timer)++;
            } else *timer = 0;
            obj->idleTimer = 0;
            return 0;
        } else {
            obj->activeTimer = 0;
            if (obj->current < 0 || obj->current != obj->expected || obj->state44 < 0) {
                if (obj->idleTimer > 70) return 1;
                obj->idleTimer++;
            } else obj->idleTimer = 0;
        }
    }
    return 0;
}
