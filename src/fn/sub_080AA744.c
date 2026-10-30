#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
#include "ram.h"
extern const s16 gUnk_086149C4[];

struct TentacleImpulseSegment {
    u8 unknown0[0x1C];
    struct ContactPosition position;
    u8 unknown24[0x105 - 0x24];
    u8 flags;
    u8 unknown106[0x11C - 0x106];
    struct ContactPosition impulse;
    u8 unknown124[4];
};
struct TentacleImpulseTrail {
    u8 unknown0[0x3D4];
    struct TentacleImpulseSegment segments[4];
    u8 unknown874[0x8D0 - 0x874];
    u16 flags;
    u8 unknown8D2[8];
    s16 count;
};

static inline s32 Tentacle_LengthSquared(struct ContactPosition *motion)
{
    return motion->x * motion->x + motion->y * motion->y + motion->z * motion->z;
}
static inline u8 Tentacle_TestSegmentFlags(struct TentacleImpulseSegment *segment, u32 input)
{
    u32 value = segment->flags;
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
static inline u8 Tentacle_TestTrailFlags(struct TentacleImpulseTrail *trail, u32 mask)
{
    if (trail->flags & mask)
        return 1;
    return 0;
}
static inline void Tentacle_MaskTrailFlags(struct TentacleImpulseTrail *trail, u32 mask)
{
    trail->flags = mask & trail->flags;
}

void sub_080AA744(struct SceneActorRegistryResetView *actor)
{
    struct TentacleImpulseTrail *trail = actor->context;
    s32 mask = Tentacle_LengthSquared(&actor->secondaryMotion);
    s32 i;
    u32 activeMask = 2;
    u32 hiddenMask = 1;
    if (mask > 1024)
        mask = 255;
    else if (mask > 128)
        mask = 127;
    else
        mask = 63;
    for (i = 0; i < trail->count; i++) {
        if (Tentacle_TestSegmentFlags(&trail->segments[i], activeMask) &&
            !Tentacle_TestSegmentFlags(&trail->segments[i], hiddenMask)) {
            u32 sample = Tentacle_Random();
            Tentacle_Polar(&trail->segments[i].impulse, sample & mask, 12);
        }
    }
    if (Tentacle_TestTrailFlags(trail, 4)) {
        Sound_PlaySong(0x3D4);
        Tentacle_MaskTrailFlags(trail, ~4);
    }
}
