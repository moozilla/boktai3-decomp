#include "global.h"
#include "ram.h"
struct ControllerSlot {
    u8 unknown0[0x14];
    u8 callback[0xE0];
    s16 state;
    u8 unknownF6[0x3AC - 0xF6];
};
struct SlotController {
    u8 unknown0[0x18];
    u32 activeMask;
    u8 unknown1C[0x38 - 0x1C];
    struct ControllerSlot slots[1];
};
void SunOrb_GetOrCreateManager(void);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
void RisingActor_LowerHeight(void);
s32 SlotController_RequestReset(s32 index)
{
    struct SlotController *controller = gSlotController;
    if (!controller) {
        SunOrb_GetOrCreateManager();
        return -1;
    }
    if (!(controller->activeMask & (1 << index)))
        return -1;
    if (controller->slots[index].state != 1)
        SunOrb_SetUpdateCallback(&controller->slots[index].callback, RisingActor_LowerHeight);
    return 0;
}
