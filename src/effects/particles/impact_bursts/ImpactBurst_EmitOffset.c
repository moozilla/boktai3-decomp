#include "global.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "scene/collision/contact_position.h"
#include "effects/particles/impact_burst.h"
extern s16 gUnk_086149C4[];
void ImpactBurst_Create(int, void *, struct AnimatedParticleConfig *);
static inline int scale(int a, int b)
{
    int v = a * 40;
    v *= b;
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
void ImpactBurst_EmitOffset(struct ContactPosition *pos, int unused, int type, int flags, int speed,
                                  int horizontal, int start, int decay)
{
    struct AnimatedParticleConfig s;
    struct ContactPosition out;
    s.type = type;
    s.flags = flags;
    s.speed = speed;
    s.vertical = 0;
    s.verticalRange = 0;
    s.horizontal = horizontal;
    s.horizontalRange = 0;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        u8 *sample = (u8 *)(offset + RANDOM_TABLE_ADDR);
        s.yaw = *sample;
    }
    s.yawRange = 0;
    s.pitchRange = 0;
    s.pitch = 0;
    s.verticalStart = start;
    s.verticalStartRange = 0;
    s.verticalDecay = decay;
    s.verticalDecayRange = 0;
    s.horizontalStart = start;
    s.horizontalStartRange = 0;
    s.horizontalDecay = decay;
    s.horizontalDecayRange = 0;
    out.x = pos->x + scale(gUnk_086149C4[(s.yaw + 192) & 255], horizontal);
    out.y = pos->y;
    out.z = pos->z + scale(gUnk_086149C4[(s.yaw + 128) & 255], horizontal);
    ImpactBurst_Create(1, &out, &s);
}
