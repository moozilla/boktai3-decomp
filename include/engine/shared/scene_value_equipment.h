#ifndef GUARD_SCENE_VALUE_EQUIPMENT_H
#define GUARD_SCENE_VALUE_EQUIPMENT_H
#include "engine/shared/global.h"
struct SceneValueEquipment {
    u8 kind, unknown1;
    u16 condition;
    u8 modifierA, unknown5[3];
    u8 modifierB, unknown9[3];
    u8 grade, gradeValue;
    u8 unknownE, unknownF;
};
#endif
