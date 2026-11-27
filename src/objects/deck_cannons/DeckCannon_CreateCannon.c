#include "global.h"
#include "objects/deck_cannons/callback_slot_layout.h"
#include "objects/deck_cannons/cannon_battery_sprite_layout.h"
#include "objects/deck_cannons/cannon_battery_position_layout.h"
struct CannonBatteryPool {
    u8 unk0[0x1C];
    u16 used;
    u8 unk1E[0xA];
    struct CallbackSlot cannons[16];
};
void DeckCannon_InitCannon(struct CallbackSlot *, s32, s32, s32, s32);
struct CallbackSlot *DeckCannon_CreateCannon(struct CannonBatteryPool *pool, s32 x, s32 y,
                                                       s32 z, s32 flip)
{
    struct CallbackSlot *cannon = NULL;
    s32 i;
    struct CallbackSlot *result;
    for (i = 0; i < 16; i++) {
        if (!((pool->used >> i) & 1)) {
            cannon = &pool->cannons[i];
            pool->used |= 1 << i;
            break;
        }
    }
    if (cannon) {
        cannon->owner = pool;
        DeckCannon_InitCannon(cannon, x, y, z, flip);
        result = cannon;
    } else {
        result = NULL;
    }
    return result;
}
