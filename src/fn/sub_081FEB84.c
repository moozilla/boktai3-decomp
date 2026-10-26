#include "global.h"
#include "ram.h"
struct ControllerSlot {
    u8 unknown0[0xC4];
    u32 flags;
    u8 unknown7C[0x12C - 0xC8];
    s16 state;
    u8 unknownE2[0x3AC - 0x12E];
};
struct SlotController {
    u8 unknown0[0x18];
    u32 activeMask;
    u8 unknown1C[0x4C - 0x1C];
    struct ControllerSlot slots[1];
};
void SunOrb_GetOrCreateManager(void);
s32 SlotController_SetSlotFlag2(s32 index)
{
    struct SlotController *controller = gSlotController;
    s32 result;
    if (!controller) {
        SunOrb_GetOrCreateManager();
        result = -1;
    } else if (!(controller->activeMask & (1 << index)))
        result = -1;
    else {
        struct ControllerSlot *slot =
            (struct ControllerSlot *)(index * sizeof(struct ControllerSlot) + (u32)controller);
        u32 mask = 2;
        slot->flags |= mask;
        result = 0;
    }
    return result;
}
