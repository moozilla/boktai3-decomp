#define gPooledControllerUpdates gUnk_08612144
#include "global.h"
#include "render.h"
struct PooledController {
    u8 unknown0;
    u8 state;
    u8 pad2[0x234 - 2];
};
struct PooledControllerManager {
    u8 task[0x18];
    u32 enabled;
    u8 pad1c[8];
    struct PooledController slots[8];
};
extern void *gUnk_08612144[];
void sub_08249244(void *, void *, s32, void *);
s32 PooledController_UpdateAll(struct PooledControllerManager *manager)
{
    struct PooledController *slot;
    s32 i;
    if (!(gEnginePauseFlags & 7)) {
        slot = manager->slots;
        for (i = 0; i <= 7; i++, slot++) {
            if (manager->enabled & (1 << i))
                sub_08249244(manager, slot, i, gUnk_08612144[slot->state]);
        }
    }
    return 0;
}
