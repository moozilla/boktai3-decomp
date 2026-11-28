#include "global.h"
#include "ui/town/shop_item_values.h"
extern struct SceneValueEquipmentDefinition gUnk_08E6092C[];
extern struct SceneValueConsumableDefinition gUnk_08E60F80[];
extern u8 gUnk_0824DB28[];
void TransformMarker_SetTypeFromTable(struct SceneValueEquipment *entry,
                                      const struct SceneValueEquipmentDefinition *definition,
                                      u32 grade);
void TransformMarker_ClearVector(struct SceneValueEquipment *entry);

void sub_0801B600(u32 equipmentSlot, u32 consumableSlot, struct SceneValueEquipment *result)
{
    struct SceneValueEquipment *entry = Inventory_GetEquipmentEntry(equipmentSlot);
    s32 id = Inventory_GetConsumableId(consumableSlot);
    struct SceneValueEquipmentDefinition *definition = &gUnk_08E6092C[entry->kind];
    struct SceneValueConsumableDefinition *item = &gUnk_08E60F80[id];
    struct SceneValueEquipmentDefinition *candidate;
    u32 next, i;

    *result = *entry;
    if (result->condition == 0) {
        u32 element = item->unknownA[1];
        next = definition->unknown18[6];
        if (next != 0xff) {
            candidate = &gUnk_08E6092C[next];
            if (candidate->unknown1 == element &&
                candidate->descriptionStyleA <= item->unknownA[0]) {
                definition = candidate;
                result->kind = definition->descriptionTitle;
                TransformMarker_SetTypeFromTable(result, definition, 0);
                return;
            }
        }
        if (definition->unknown1 != element) {
            TransformMarker_SetTypeFromTable(result, definition, 0);
            TransformMarker_ClearVector(result);
        }
    } else if (definition->descriptionStyleA > item->unknownA[0]) {
        if (definition->unknown1 != item->unknownA[1]) {
            next = definition->unknown18[4];
            if (next != 0xff) {
                definition = &gUnk_08E6092C[next];
                result->kind = definition->descriptionTitle;
                TransformMarker_SetTypeFromTable(result, definition, 0);
            }
        }
    } else {
        s32 grade;
        u32 element = item->unknownA[1];
        for (i = 0; i < 5; i++) {
            next = definition->unknown18[gUnk_0824DB28[i]];
            if (next != 0xff) {
                candidate = &gUnk_08E6092C[next];
                if (candidate->unknown1 == element &&
                    candidate->descriptionStyleA <= item->unknownA[0]) {
                    definition = candidate;
                    result->kind = definition->descriptionTitle;
                    TransformMarker_SetTypeFromTable(result, definition, 0);
                    return;
                }
            }
        }
        grade = result->grade;
        if (grade <= 1)
            grade++;
        TransformMarker_SetTypeFromTable(result, definition, grade);
    }
}
