#include "global.h"
struct PooledController {
    u8 active, state;
    u8 pad2[4];
    u16 timer;
    u8 pad8[0x234 - 8];
};
struct PooledControllerManager {
    u8 task[0x18];
    u32 enabled;
    u8 pad1c[8];
    struct PooledController slots[8];
};
s32 PooledController_ActivateIdleSlots(struct PooledControllerManager *manager)
{
    struct PooledController *slot = manager->slots;
    s32 i;
    for (i = 0; i <= 7; i++, slot++) {
        if ((manager->enabled & (1 << i)) && slot->state == 0) {
            slot->state = 3;
            slot->active = 1;
            slot->timer = 0;
        }
    }
}
