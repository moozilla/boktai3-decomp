#ifndef GUARD_SCENE_CONSUMABLE_DEFINITION_H
#define GUARD_SCENE_CONSUMABLE_DEFINITION_H

#include "global.h"

struct SceneConsumableEffect {
    u8 kind, amount;
} __attribute__((packed));

struct SceneValueConsumableDefinition {
    u8 unknown0;
    struct SceneConsumableEffect effects[2];
    u8 unknown5[3];
    s16 baseValue;
    u8 unknownA[2], slot, unknownD[3];
};

#endif
