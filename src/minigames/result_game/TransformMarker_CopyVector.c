#include "ui/town/shop_item_values.h"

void TransformMarker_CopyVector(struct SceneValueEquipment *entry,
                                const struct SceneValueEquipmentDefinition *definition)
{
    entry->modifierA = definition->modifierTextA;
    entry->unknown5[0] = definition->unknownD[0];
    entry->unknown5[1] = definition->unknownD[1];
    entry->unknown5[2] = definition->unknownD[2];
    entry->modifierB = definition->modifierTextB;
    entry->unknown9[0] = definition->unknown11[0];
    entry->unknown9[1] = definition->unknown11[1];
    entry->unknown9[2] = definition->unknown11[2];
}
