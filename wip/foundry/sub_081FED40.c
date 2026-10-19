#include "global.h"
#define SlotController_StopSlotAndSong sub_081FED40
#define SlotController_SetCallback     sub_081FE928
struct SlotControl {
    u8 unknown0[6];
    u16 flags;
};
struct ControllerSlot {
    u8 callback[0x8C];
    struct SlotControl control;
    u8 unknownE0[0x3AC - 0x94];
};
struct SlotController {
    u8 unknown0[0x18];
    u32 activeMask;
    u8 unknown1C[0x4C - 0x1C];
    struct ControllerSlot slots[4];
};
void sub_081FDAAC(void);
void sub_081FE928(void *, void (*)(void));
extern struct SlotController *gUnk_020005EC;
#define gSlotController gUnk_020005EC
void Sound_StopSong(u32);
s32 sub_081FED40(s32 index)
{
    struct SlotController *controller = gUnk_020005EC;
    s32 result;
    if (!controller || !(controller->activeMask & (1 << index)))
        result = -1;
    else {
        struct SlotControl *control = &controller->slots[index].control;
        control->flags |= 4;
        sub_081FE928((u8 *)controller + index * 0x3AC + 0x4C, sub_081FDAAC);
        Sound_StopSong(0x564);
        result = 0;
    }
    return result;
}
