#include "scene_input_compare.h"
void SceneInputPanel_GetEquippedValues(s32 *primaryValue, s32 *secondaryValue,
                                       struct SceneInputItemDescriptor *primaryDescriptor,
                                       u32 *secondaryDescriptor)
{
    struct SceneInputItemDescriptor temporary;
    s32 first = 0, second = 0;
    s32 secondIndex = -1, firstIndex = -1;
    struct SceneInputEquipmentView *equipment = (void *)gGameState;
    s32 index;
    if (equipment->selected <= 7) {
        struct SceneInputEquipmentView *entry;
        index = equipment->selected;
        entry = (void *)(index * 2 + (u32)equipment);
        if (entry->primary[0] >= 0) {
            Inventory_GetEquippedDescriptor(index, &temporary);
            first = Player_GetEquipmentValue(&temporary);
            firstIndex = index;
        }
    } else {
        struct SceneInputEquipmentView *entry;
        index = equipment->selected - 8;
        entry = (void *)(index * 2 + (u32)equipment);
        if (entry->secondary[0] >= 0) {
            second =
                Player_GetEquipmentLevelBonus(GameState_GetUnlockDescriptor(entry->secondary[0]));
            secondIndex = index;
        }
    }
    if (primaryValue)
        *primaryValue = first;
    if (secondaryValue)
        *secondaryValue = second;
    if (primaryDescriptor && firstIndex >= 0)
        Inventory_GetEquippedDescriptor(firstIndex, primaryDescriptor);
    if (secondaryDescriptor && secondIndex >= 0) {
        struct SceneInputEquipmentView *entry;
        struct SceneInputEquipmentView *reloaded = (void *)gGameState;
        entry = (void *)(secondIndex * 2 + (u32)reloaded);
        *secondaryDescriptor = *(u32 *)GameState_GetUnlockDescriptor(entry->secondary[0]);
    }
}
