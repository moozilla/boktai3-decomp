#include "global.h"
#define CallbackSlot_AdvancePosition sub_08199BD8
struct CallbackSlot { u8 unknown0[0x24]; u32 flags; u8 unknown28[0x18]; u16 position[3]; u8 unknown46[0x8a]; u16 timer; u8 phase; u8 state; };
void sub_08199BD8(struct CallbackSlot *slot)
{
    u16 *component;
    if (slot->flags & 4)
        component = &slot->position[0];
    else
        component = &slot->position[2];
    *component += 20;
    if (++slot->timer > 2) {
        u32 state = 3;
        slot->state = state;
        slot->phase = 0;
        slot->timer = 0;
    }
}
