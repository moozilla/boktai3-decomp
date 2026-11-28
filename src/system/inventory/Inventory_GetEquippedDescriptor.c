#include "system/game_state/game_state.h"
void ItemDescriptor_Reset(void *);
void Inventory_GetEquipmentDescriptor(s32, void *);
void Inventory_GetEquippedDescriptor(u32 i, void *descriptor)
{
    if (*(i + gGameState->inventory) < 0)
        ItemDescriptor_Reset(descriptor);
    else
        Inventory_GetEquipmentDescriptor(*(i + gGameState->inventory), descriptor);
}
