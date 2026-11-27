#include "global.h"
#include "effects/particles/impact_burst.h"
/* ImpactBurst_EmitAngledBurst: Builds a particle Config and emits a single burst with vertical/horizontal/pitch angle ranges (matches existing src alias) */
void ImpactBurst_Create(int, void *, struct AnimatedParticleConfig *);
void ImpactBurst_EmitAngledBurst(void *pos, int type, int flags, int speed, int vertical,
                                       int verticalRange, int horizontal, int horizontalRange,
                                       int pitch, int pitchRange)
{
    struct AnimatedParticleConfig s;
    s.type = type;
    s.flags = flags;
    s.speed = speed;
    s.vertical = vertical;
    s.verticalRange = verticalRange;
    s.horizontal = horizontal;
    s.horizontalRange = horizontalRange;
    s.pitch = pitch;
    s.pitchRange = pitchRange;
    s.yaw = 0;
    s.yawRange = 255;
    s.verticalStart = 255;
    s.verticalStartRange = 0;
    s.verticalDecay = 255;
    s.verticalDecayRange = 0;
    s.horizontalStart = 255;
    s.horizontalStartRange = 0;
    s.horizontalDecay = 255;
    s.horizontalDecayRange = 0;
    ImpactBurst_Create(1, pos, &s);
}
