#include "global.h"
#include "effects/particles/impact_burst.h"
int Div(int, int);
void ImpactBurst_Create(int, void *, struct AnimatedParticleConfig *);
void ImpactBurst_EmitRing(void *pos, int count, int type, int flags, int speed,
                                int horizontal, int start, int decay)
{
    struct AnimatedParticleConfig s;
    int step, i;
    s.type = type;
    s.flags = flags;
    s.speed = speed;
    s.vertical = 0;
    s.verticalRange = 0;
    s.horizontal = horizontal;
    s.horizontalRange = 0;
    step = Div(256, count);
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
    s.yawRange = 10;
    s.yaw = 0;
    for (i = 0; i < count; i++) {
        ImpactBurst_Create(1, pos, &s);
        s.yaw += step;
    }
}
