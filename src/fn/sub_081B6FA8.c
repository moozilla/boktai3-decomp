#define PooledController_UpdateAll sub_081B6FA8
#define gEnginePauseFlags          gUnk_0300523C
#define gPooledControllerUpdates   gUnk_08612144
#include "global.h"
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
extern u32 gUnk_0300523C;
extern void *gUnk_08612144[];
void sub_08249244(void *, void *, s32, void *);
s32 sub_081B6FA8(struct PooledControllerManager *manager)
{
    struct PooledController *slot;
    s32 i;
    if (!(gUnk_0300523C & 7)) {
        slot = manager->slots;
        for (i = 0; i <= 7; i++, slot++) {
            if (manager->enabled & (1 << i))
                sub_08249244(manager, slot, i, gUnk_08612144[slot->state]);
        }
    }
    return 0;
}
