#include "global.h"
#include "breakable_prop.h"
#include "scene_vec.h"
#include "engine.h"
#include "ram.h"

static inline u16 Particles_NextRandom(u32 *indexAddress, const u16 *table, u32 limit)
{
    *indexAddress = (*indexAddress + 1) & limit;
    return table[*indexAddress];
}
void AnimatedParticles_EmitRandomBurst(struct SceneVec *base, struct SceneVec *mask, s32 yaw,
                                       s32 yawRange, s32 count, s32 type, s32 flags, s32 speed,
                                       s32 horizontal, s32 start, s32 decay)
{
    struct AnimatedParticleConfig config;
    struct SceneVec pos;
    s32 remaining;
    u32 *indexAddress;
    u32 limit;
    const u16 *table;
    s16 horizontalValue = horizontal;
    config.type = type;
    config.flags = flags;
    config.speed = speed;
    config.vertical = 0;
    config.verticalRange = 0;
    config.horizontal = horizontalValue;
    config.horizontalRange = 0;
    config.pitch = 0;
    config.pitchRange = 0;
    config.yaw = yaw;
    config.yawRange = yawRange;
    config.verticalStart = start;
    config.verticalStartRange = 0;
    config.verticalDecay = decay;
    config.verticalDecayRange = 0;
    config.horizontalStart = start;
    config.horizontalStartRange = 0;
    config.horizontalDecay = decay;
    config.horizontalDecayRange = 0;
    if (count > 0) {
        indexAddress = &gRandomIndex;
        limit = 1023;
        remaining = count;
        do {
            table = gRandomTable;
            pos.x = (Particles_NextRandom(indexAddress, table, limit) & mask->x) + base->x;
            pos.y = (Particles_NextRandom(indexAddress, table, limit) & mask->y) + base->y;
            pos.z = (Particles_NextRandom(indexAddress, table, limit) & mask->z) + base->z;
            AnimatedParticles_Create(1, &pos, &config);
            table = 0;
        } while (--remaining);
    }
}
