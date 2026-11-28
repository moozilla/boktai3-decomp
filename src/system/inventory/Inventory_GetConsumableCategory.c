#include "ui/town/scene_consumable_definition.h"
#include "system/game_state/game_state.h"
struct SceneValueConsumableDefinition *GameState_GetConsumableDescriptor(s32);
u8 Inventory_GetConsumableCategory(u32 i)
{
    return GameState_GetConsumableDescriptor(*(i + gGameState->table160))->slot;
}
