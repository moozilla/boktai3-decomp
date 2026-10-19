#include "global.h"
#define SlotController_RequestStop sub_081FEC38
#define SlotController_SetCallback sub_081FE928
struct SlotControl {
    u8 unknown0[6];
    u16 flags;
};
struct ControllerSlot {
    u8 unknown0[0x4C];
    u8 callback[0x8C];
    struct SlotControl control;
    u8 unknownE0[0x3AC - 0xE0];
};
struct SlotController {
    u8 unknown0[0x18];
    u32 activeMask;
};
void sub_081FDAAC(void);
void sub_081FE928(void *, void (*)(void));
s32 sub_081FEC38(struct SlotController *controller, s32 index)
{
    s32 result;
    if (!(controller->activeMask & (1 << index)))
        result = -1;
    else {
        struct SlotControl *control = &((struct ControllerSlot *)controller)[index].control;
        control->flags |= 4;
        sub_081FE928(((struct ControllerSlot *)controller)[index].callback, sub_081FDAAC);
        result = 0;
    }
    return result;
}
