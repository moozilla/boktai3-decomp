#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.2%
// foundry draft (worker best 87.5%); what differs is mostly register allocation and instruction order
#define CallbackSlot_RestorePosition sub_08199C20
struct CallbackPosition { u32 x, z; };
struct CallbackSlot { u8 unknown0[0x24]; u32 flags; u8 unknown28[0x18]; struct CallbackPosition position; u8 unknown48[0x5c]; struct CallbackPosition savedPosition; u8 unknownAC[0x24]; u16 timer; u8 phase; u8 state; };
void sub_08199C20(struct CallbackSlot *slot)
{
    u16 *component;
    if (slot->flags & 4)
        component = (u16 *)&slot->position.x;
    else
        component = (u16 *)&slot->position.z;
    *component -= 3;
    if (++slot->timer > 19) {
        slot->position = slot->savedPosition;
        slot->state = 0;
        slot->phase = 0;
        slot->timer = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08199C20);
#endif
