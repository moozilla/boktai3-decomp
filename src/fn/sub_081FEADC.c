#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.2%
// foundry draft (worker best 93.2%); what differs is mostly register allocation and instruction order
#define SlotController_RequestReset sub_081FEADC
#define gSlotController gUnk_020005EC
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
extern struct SlotController *gUnk_020005EC;
void SunOrb_GetOrCreateManager(void);
void SunOrb_SetUpdateCallback(void *, void (*)(void));
void RisingActor_LowerHeight(void);
s32 sub_081FEADC(s32 index)
{
    struct SlotController *controller = gUnk_020005EC;
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
#else
INCLUDE_ASM("asm/nonmatching", sub_081FEADC);
#endif
