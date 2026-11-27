#include "global.h"
#include "effects/particles/impact_burst.h"
void ImpactBurst_Create(int, void *, struct AnimatedParticleConfig *);
void ImpactBurst_EmitDirectional(int count, int type, int flags, int speed, int yaw,
                                       int yawRange, int horizontal, int horizontalRange,
                                       int vertical, int verticalRange, void *pos)
{
    struct AnimatedParticleConfig s;
    s.type = type;
    s.flags = flags;
    s.speed = speed;
    s.vertical = vertical;
    s.verticalRange = verticalRange;
    s.horizontal = horizontal;
    s.horizontalRange = horizontalRange;
    s.pitch = 60;
    s.pitchRange = 8;
    s.yaw = yaw;
    s.yawRange = yawRange;
    s.verticalStart = 255;
    s.verticalStartRange = 0;
    s.verticalDecay = 255;
    s.verticalDecayRange = 0;
    s.horizontalStart = 255;
    s.horizontalStartRange = 0;
    s.horizontalDecay = 255;
    s.horizontalDecayRange = 0;
    ImpactBurst_Create(count, pos, &s);
}
