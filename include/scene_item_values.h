#ifndef GUARD_SCENE_ITEM_VALUES_H
#define GUARD_SCENE_ITEM_VALUES_H
#include "engine.h"
/* Tentative inventory-value family: the callers select equipment/item slots. */
struct SceneValueEquipment {
    u8 kind, unknown1;
    u16 condition;
    u8 modifierA, unknown5[3];
    u8 modifierB, unknown9[3];
    u8 grade, unknownD[3];
};
struct SceneValueEquipmentDefinition {
    u8 descriptionTitle, unknown1, descriptionStyleA, descriptionStyleB;
    u8 unknown4[2];
    u16 baseValue;
    u8 unknown8[4];
    u8 modifierTextA, unknownD[3];
    u8 modifierTextB, unknown11[15];
};
struct SceneValueItemDefinition {
    u16 unknown0;
    u16 actionKind; /* 0x2: selected-action dispatch kind */
    u16 conditionKey, unknown6;
    u32 unknown8, baseValue;
};
struct SceneValueConsumableDefinition {
    u8 unknown0[8];
    s16 baseValue;
    u8 unknownA[6];
};
u32 Inventory_GetEquipmentId(u32 slot);
u32 Inventory_CanAssignEquipment(u32 slot);
struct SceneValueEquipment *Inventory_GetEquipmentEntry(u32);
struct SceneValueEquipmentDefinition *Equipment_GetDefinition(u32);
struct SceneValueConsumableDefinition *GameState_GetConsumableDescriptor(u32);
s32 SceneInventory_GetItemConditionFrame(u32);
s32 Inventory_GetItemId(u32);
s32 Inventory_GetConsumableId(u32);
u32 SceneItemValue_GetItemBaseValue(s32);
s32 SceneItemValue_GetConsumableBaseValue(u32);
#endif
