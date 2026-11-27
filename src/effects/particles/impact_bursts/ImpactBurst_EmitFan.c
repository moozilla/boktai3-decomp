#include "global.h"
#include "effects/particles/impact_burst.h"
/* ImpactBurst_EmitFan: Builds a particle Config and emits a fan-shaped spread using a yaw/yawRange/count/decay parameter set (matches existing src alias) */
void ImpactBurst_Create(int, void *, struct AnimatedParticleConfig *);
void ImpactBurst_EmitFan(void *pos, int yaw, int yawRange, int count, int type, int flags,
                               int speed, int horizontal, int start, int decay)
{
    struct AnimatedParticleConfig s;
    s.type = type;
    s.flags = flags;
    s.speed = speed;
    s.vertical = 0;
    s.verticalRange = 0;
    s.horizontal = horizontal;
    s.horizontalRange = 0;
    s.pitch = 0;
    s.pitchRange = 0;
    s.yaw = yaw;
    s.yawRange = yawRange;
    s.verticalStart = start;
    s.verticalStartRange = 0;
    s.verticalDecay = decay;
    s.verticalDecayRange = 0;
    s.horizontalStart = start;
    s.horizontalStartRange = 0;
    s.horizontalDecay = decay;
    s.horizontalDecayRange = 0;
    ImpactBurst_Create(count, pos, &s);
}
