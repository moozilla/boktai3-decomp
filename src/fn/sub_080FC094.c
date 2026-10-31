#include "global.h"
#include "contact_actor.h"
#include "scene_actor_reset.h"

extern const s16 gUnk_086149C4[];
void SceneParticleBurst_CreateWithPause(const void *, const void *, const void *, s32, s32, s32);

struct Burst080FC094Drawable {
    u8 unk_00[6];
    u8 heading;
    u8 unk_07[0x15];
    struct ContactPosition position;
};

static inline s32 Burst080FC094_Scale(s32 value)
{
    s32 result;
    if (value >= 0)
        result = value >> 12;
    else
        result = -(-value >> 12);
    return result;
}

// confidence=low: spawns a particle burst at a point offset from the drawable by its heading.
void sub_080FC094(struct SceneActorRegistryResetView *actor, s32 count)
{
    struct Burst080FC094Drawable *drawable = actor->drawable;
    struct ContactPackedVector range;
    struct ContactPosition offset;
    struct ContactPosition velocity;
    union {
        struct ContactPosition p;
        struct ContactPackedVector v;
    } position;
    s32 x, z, x2, z2;

    range.x = 120;
    range.y = 60;
    range.z = 120;
    position.p = drawable->position;
    ZeroFill8(&offset);
    ZeroFill8(&velocity);
    offset.x = -140;
    offset.z = -140;
    x = Burst080FC094_Scale(offset.x * gUnk_086149C4[(drawable->heading + 64) & 255]);
    z = Burst080FC094_Scale(offset.z * gUnk_086149C4[drawable->heading]);
    position.v.x = x + z + position.p.x;
    x2 = Burst080FC094_Scale(-(offset.x * gUnk_086149C4[drawable->heading]));
    z2 = Burst080FC094_Scale(offset.z * gUnk_086149C4[(drawable->heading + 64) & 255]);
    position.v.z = x2 + z2 + position.p.z;
    SceneParticleBurst_CreateWithPause(&position, &velocity, &range, count, 2, 4);
}
