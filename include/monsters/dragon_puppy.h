#ifndef GUARD_DRAGON_PUPPY_H
#define GUARD_DRAGON_PUPPY_H

#include "scene/collision/contact_swarm.h"

struct DragonPuppy {
    struct Task task;
    u32 ticks;
    struct ContactPosition initialPosition;
    struct ContactSwarmSprite sprite;
    u8 palette[0x1C];
    struct ContactCollider collider;
    u8 terrainBody[0x48];
    void *update;
    struct DisplaySequenceState animation;
    u32 unknown11C;
    s16 attackX, attackY, attackZ;
    s16 health, damage, flashEnabled, unknown12C, defeated, flashTimer;
    u8 deathVariant, unknown133;
    s16 attackInterval, unknown136, maxHealth, unknown13A, bodyReactionFlags, sparkMask;
    s16 hudValue140, showTerrainBody, unknown144, unknown146, hudValue148, initialDead;
    void *context;
    u32 stateWord, savedStateWord, unknown158, unknown15C, unknown160, attackSchedule;
    u32 unknown168, terrainEnabled;
    void *deathAnimation;
    u8 terrain[0x24];
};

typedef char DragonPuppySizeCheck[sizeof(struct DragonPuppy) == 0x198 ? 1 : -1];

#endif
