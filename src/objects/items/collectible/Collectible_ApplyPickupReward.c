#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"
#include "engine/engine_pause.h"

void GameState_AddPickupRewardCount(u32);

void Collectible_ApplyPickupReward(struct Collectible *item, struct SpatialEntry *other)
{
    if (Engine_PauseBits(0x800) == 0 && other->mask == 1) {
        switch (item->type) {
            case 0:
                GameState_AddPickupRewardCount(1);
                break;
            case 1:
                GameState_AddPickupRewardCount(2);
                break;
        }
    }
}
