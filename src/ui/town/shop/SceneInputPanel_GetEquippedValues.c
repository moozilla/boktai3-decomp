#include "ui/widgets/scene_input_compare.h"
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
        index = equipment->selected;
        if (*(index + equipment->primary) >= 0) {
            Inventory_GetEquippedDescriptor(index, &temporary);
            first = Player_GetEquipmentValue(&temporary);
            firstIndex = index;
        }
    } else {
        index = equipment->selected - 8;
        if (*(index + equipment->secondary) >= 0) {
            second = Player_GetEquipmentLevelBonus(
                GameState_GetUnlockDescriptor(*(index + equipment->secondary)));
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
        struct SceneInputEquipmentView *reloaded = (void *)gGameState;
        *secondaryDescriptor =
            *(u32 *)GameState_GetUnlockDescriptor(*(secondIndex + reloaded->secondary));
    }
}
