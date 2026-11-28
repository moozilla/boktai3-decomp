#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

void sub_08104748(struct CallbackPoolEntry *entry)
{
    struct CallbackPoolEntry *self = entry;
    if (entry->collider.responseValue) {
        entry->collider.flags |= 4;
        entry->collider.responseValue--;
    } else {
        entry->collider.flags &= ~4;
    }
    if (self->hitTimer) {
        AnimData_SelectPaletteBank((void *)entry->sprite.resource, 0x192);
        self->hitTimer--;
    } else {
        AnimData_SelectPaletteBank((void *)entry->sprite.resource, 0x1EA);
    }
}
