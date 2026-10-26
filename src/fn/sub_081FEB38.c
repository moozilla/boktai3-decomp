#include "global.h"
#include "ram.h"
struct ControllerSlot {
    u8 callback[0xE0];
    s16 state;
    u8 unknownE2[0x3AC - 0xE2];
};
struct SlotController {
    u8 unknown0[0x18];
    u32 activeMask;
    u8 unknown1C[0x4C - 0x1C];
    struct ControllerSlot slots[1];
};
void SunOrb_GetOrCreateManager(void);
s32 SlotController_IsSlotIdle(s32 index)
{
    struct SlotController *controller = gSlotController;
    if (!controller) {
        SunOrb_GetOrCreateManager();
    } else if (controller->activeMask & (1 << index)) {
        if (controller->slots[index].state == 0)
            return 1;
        return 0;
    }
    return -1;
}
