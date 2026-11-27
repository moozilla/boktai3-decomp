#include "global.h"
#include "objects/deck_cannons/callback_slot_layout.h"
struct CallbackPoolOwner {
    u8 unknown0[0x24];
    u16 delay;
};
void CallbackSlot_StartWhenTimerReady(void *unused0, void *unused1, struct CallbackSlot *slot)
{
    struct CallbackPoolOwner *owner = slot->owner;
    if (slot->state == 0) {
        if (slot->timer >= owner->delay) {
            slot->state = 1;
            slot->phase = 0;
            slot->timer = 0;
        }
    }
}
