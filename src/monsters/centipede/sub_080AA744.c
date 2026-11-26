#include "monsters/centipede/tentacle_body.h"
#include "global.h"
#include "scene/actor/scene_actor_reset.h"
#include "engine/engine.h"
#include "engine/ram.h"
extern const s16 gUnk_086149C4[];

static inline s32 Tentacle_LengthSquared(struct ContactPosition *motion)
{
    return motion->x * motion->x + motion->y * motion->y + motion->z * motion->z;
}
static inline u8 Tentacle_TestSegmentFlags(struct TentacleAnimationSegment *segment, u32 input)
{
    u32 value = segment->animation.status;
    u32 mask = input;
    if (mask & value)
        return 1;
    return 0;
}
static inline s32 Tentacle_TruncateFixed12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline void Tentacle_Polar(struct ContactPosition *out, u16 angle, s32 speed)
{
    out->x = Tentacle_TruncateFixed12(gUnk_086149C4[(angle + 64) & 255] * speed);
    out->y = 0;
    out->z = Tentacle_TruncateFixed12(gUnk_086149C4[angle & 255] * speed);
}
static inline u16 Tentacle_Random(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return gRandomTable[index];
}
static inline u8 Tentacle_TestTrailFlags(struct TentacleBody *trail, u32 mask)
{
    if (trail->attackFlags & mask)
        return 1;
    return 0;
}
static inline void Tentacle_MaskTrailFlags(struct TentacleBody *trail, u32 mask)
{
    trail->attackFlags = mask & trail->attackFlags;
}

void sub_080AA744(struct ChaseActor *actor)
{
    struct TentacleBody *trail = actor->base.context;
    s32 mask = Tentacle_LengthSquared(&actor->base.secondaryMotion);
    s32 i;
    u32 activeMask = 2;
    u32 hiddenMask = 1;
    if (mask > 1024)
        mask = 255;
    else if (mask > 128)
        mask = 127;
    else
        mask = 63;
    for (i = 0; i < trail->segmentCount; i++) {
        if (Tentacle_TestSegmentFlags(&trail->segments[i], activeMask) &&
            !Tentacle_TestSegmentFlags(&trail->segments[i], hiddenMask)) {
            u32 sample = Tentacle_Random();
            Tentacle_Polar(&trail->segments[i].offset, sample & mask, 12);
        }
    }
    if (Tentacle_TestTrailFlags(trail, 4)) {
        Sound_PlaySong(0x3D4);
        Tentacle_MaskTrailFlags(trail, ~4);
    }
}
