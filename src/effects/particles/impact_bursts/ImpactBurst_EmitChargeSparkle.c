#include "global.h"
#include "effects/particles/impact_burst.h"
void ImpactBurst_Create(u32, u32, struct AnimatedParticleConfig *);
void ImpactBurst_EmitChargeSparkle(u32 a, u32 b, u32 c, u32 d, u32 e)
{
    struct AnimatedParticleConfig s;
    s.type = b;
    s.flags = c;
    s.speed = d;
    s.vertical = 0xc;
    s.verticalRange = 0xc;
    s.horizontal = 4;
    s.horizontalRange = 4;
    s.pitch = 0x3c;
    s.pitchRange = 8;
    s.yaw = 0;
    s.yawRange = 0xff;
    s.verticalStart = 0xff;
    s.verticalStartRange = 0;
    s.verticalDecay = 0xff;
    s.verticalDecayRange = 0;
    s.horizontalStart = 0xff;
    s.horizontalStartRange = 0;
    s.horizontalDecay = 0xff;
    s.horizontalDecayRange = 0;
    ImpactBurst_Create(a, e, &s);
}
