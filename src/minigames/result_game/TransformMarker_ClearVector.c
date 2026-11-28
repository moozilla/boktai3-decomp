#include "scene/scene_value_equipment.h"

void TransformMarker_ClearVector(struct SceneValueEquipment *entry)
{
    entry->modifierA = 0;
    entry->unknown5[0] = 0;
    entry->unknown5[1] = 0;
    entry->unknown5[2] = 0;
    entry->modifierB = 0;
    entry->unknown9[0] = 0;
    entry->unknown9[1] = 0;
    entry->unknown9[2] = 0;
}
