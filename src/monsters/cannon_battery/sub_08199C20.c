#include "engine/shared/global.h"
#include "engine/shared/contact_position.h"
struct CallbackSlot {
    u8 unknown0[0x24];
    u32 flags;
    u8 unknown28[0x18];
    struct ContactPosition position;
    u8 unknown48[0x5c];
    struct ContactPosition savedPosition;
    u8 unknownAC[0x24];
    u16 timer;
    u8 phase;
    u8 state;
};
void CallbackSlot_RestorePosition(struct CallbackSlot *slot)
{
    if (slot->flags & 4)
        slot->position.x -= 3;
    else
        slot->position.z -= 3;
    if (++slot->timer > 19) {
        slot->position = slot->savedPosition;
        slot->state = 0;
        slot->phase = 0;
        slot->timer = 0;
    }
}
