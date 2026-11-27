#include "global.h"
#include "scene/collision/contact_position.h"
#include "objects/deck_cannons/callback_slot_layout.h"
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
