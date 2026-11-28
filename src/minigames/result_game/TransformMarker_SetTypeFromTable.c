#include "ui/town/shop_item_values.h"

void TransformMarker_SetTypeFromTable(struct SceneValueEquipment *entry,
                                      const struct SceneValueEquipmentDefinition *definition,
                                      u32 grade)
{
    entry->grade = grade;
    entry->gradeValue = definition->gradeValues[grade];
    entry->unknownE = 0;
    entry->unknownF = 0;
}
