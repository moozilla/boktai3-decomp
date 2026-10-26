#include "global.h"
#include "breakable_prop.h"
#include "scene_vec.h"
#include "engine.h"

extern const u16 gUnk_0203B400[];
static inline u16 Particles_NextRandom(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return gUnk_0203B400[gRandomIndex];
}
#ifdef NONMATCHING
// fuzzy: 64.0%
// scope shape 22 (differences once low registers are set aside)
// scope shape 38 (differences once low registers are set aside)
void sub_08015A0C(struct SceneVec *base, struct SceneVec *mask, s32 yaw, s32 yawRange, s32 count,
                  s32 type, s32 flags, s32 speed, s16 horizontal, s32 start, s32 decay)
{
    struct AnimatedParticleConfig config;
    struct SceneVec pos;
    s32 i;
    config.type = type;
    config.flags = flags;
    config.speed = speed;
    config.vertical = 0;
    config.verticalRange = 0;
    config.horizontal = horizontal;
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
    for (i = count; i > 0; i--) {
        pos.x = (Particles_NextRandom() & mask->x) + base->x;
        pos.y = (Particles_NextRandom() & mask->y) + base->y;
        pos.z = (Particles_NextRandom() & mask->z) + base->z;
        AnimatedParticles_Create(1, &pos, &config);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08015A0C);
#endif
