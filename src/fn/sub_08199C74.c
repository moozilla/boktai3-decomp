#include "global.h"
struct CallbackPoolOwner {
    u8 unknown0[0x24];
    u16 delay;
};
struct CallbackSlot {
    u8 unknown0[0xac];
    struct CallbackPoolOwner *owner;
    u8 unknownB0[0x20];
    u16 timer;
    u8 phase;
    u8 state;
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
