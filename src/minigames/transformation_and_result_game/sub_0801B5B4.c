#include "engine/shared/scene_item_values.h"

void TransformMarker_SetTypeFromTable(u8 *result, u8 *table, u32 grade)
{
    struct SceneValueEquipment *entry = (struct SceneValueEquipment *)result;
    const struct SceneValueEquipmentDefinition *definition =
        (const struct SceneValueEquipmentDefinition *)table;
    const u8 *values;

    entry->grade = grade;
    values = definition->gradeValues;
    entry->gradeValue = values[grade];
    entry->unknownE = 0;
    entry->unknownF = 0;
}
