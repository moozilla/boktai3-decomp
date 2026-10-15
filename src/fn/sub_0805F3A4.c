#include "scene_input_compare.h"
#define SceneInputPanel_GetEquippedValues sub_0805F3A4
void SceneInputPanel_GetEquippedValues(s32 *primaryValue, s32 *secondaryValue,
                                       struct SceneInputItemDescriptor *primaryDescriptor,
                                       u32 *secondaryDescriptor)
{
    struct SceneInputItemDescriptor temporary;
    s32 first = 0, second = 0;
    s32 secondIndex = -1, firstIndex = -1;
    struct SceneInputEquipmentView *equipment = (void *)gUnk_02000710;
    s32 index;
    if (equipment->selected <= 7) {
        struct SceneInputEquipmentView *entry;
        index = equipment->selected;
        entry = (void *)(index * 2 + (u32)equipment);
        if (entry->primary[0] >= 0) {
            sub_0822D808(index, &temporary);
            first = sub_081590E4(&temporary);
            firstIndex = index;
        }
    } else {
        struct SceneInputEquipmentView *entry;
        index = equipment->selected - 8;
        entry = (void *)(index * 2 + (u32)equipment);
        if (entry->secondary[0] >= 0) {
            second = sub_081590FC(sub_0822E5F4(entry->secondary[0]));
            secondIndex = index;
        }
    }
    if (primaryValue)
        *primaryValue = first;
    if (secondaryValue)
        *secondaryValue = second;
    if (primaryDescriptor && firstIndex >= 0)
        sub_0822D808(firstIndex, primaryDescriptor);
    if (secondaryDescriptor && secondIndex >= 0) {
        struct SceneInputEquipmentView *entry;
        struct SceneInputEquipmentView *reloaded = (void *)gUnk_02000710;
        entry = (void *)(secondIndex * 2 + (u32)reloaded);
        *secondaryDescriptor = *(u32 *)sub_0822E5F4(entry->secondary[0]);
    }
}
