#include "global.h"
#include "link/panel/link_slot_display_element.h"

struct Unk0810C1E8 {
    u8 filler0[0x28];
    struct LinkSlotDisplayElement elems[7];
    u8 filler2C8[0x38];
    u16 vals[8]; // 0x300
};

void LinkPanelSlots_ShowSlotReadyFrame(struct Unk0810C1E8 *p, s32 i)
{
    p->elems[i].flags &= ~1;
    p->vals[i] = 0x2c;
}
