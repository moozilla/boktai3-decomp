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
    u8 unknown0[6];
    u16 baseValue;
    u8 unknown8[0x18];
};
struct SceneValueItemDefinition {
    u32 unknown0[3], baseValue;
};
struct SceneValueConsumableDefinition {
    u8 unknown0[8];
    s16 baseValue;
    u8 unknownA[6];
};
struct SceneValueEquipment *sub_0822D6C8(u32);
struct SceneValueEquipmentDefinition *sub_08227F5C(u32);
struct SceneValueConsumableDefinition *sub_0822E288(u32);
s32 sub_0816D820(u32);
s32 sub_0822CDA8(u32);
s32 sub_0822E104(u32);
u32 sub_0805F86C(s32);
s32 sub_0805F8D8(u32);
#endif
