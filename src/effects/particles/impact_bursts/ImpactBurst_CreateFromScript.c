#include "global.h"
#include "ui/hud/scene_indicator.h"
#include "effects/particles/impact_burst.h"
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
int Script_GetValue(void);
void ImpactBurst_Create(int, void *, struct AnimatedParticleConfig *);
void ImpactBurst_CreateFromScript(void)
{
    struct AnimatedParticleConfig s;
    struct Vec pos;
    int count = Script_GetKeywordValue('n', 1);
    if (Script_SeekToKeyword('p')) {
        pos.x = Script_GetValue();
        pos.y = Script_GetValue();
        pos.z = Script_GetValue();
    } else {
        pos.x = 0;
        pos.y = 0;
        pos.z = 0;
    }
    s.type = Script_GetKeywordValue('t', 0);
    s.flags = Script_GetKeywordValue('f', 0);
    s.speed = Script_GetKeywordValue('a', 64);
    if (Script_SeekToKeyword('s')) {
        s.vertical = Script_GetValue();
        s.verticalRange = Script_GetValue();
        s.horizontal = Script_GetValue();
        s.horizontalRange = Script_GetValue();
    } else {
        s.vertical = 0;
        s.verticalRange = 0;
        s.horizontal = 0;
        s.horizontalRange = 0;
    }
    if (Script_SeekToKeyword('d')) {
        s.pitch = Script_GetValue();
        s.pitchRange = Script_GetValue();
        s.yaw = Script_GetValue();
        s.yawRange = Script_GetValue();
    } else {
        s.pitch = 0;
        s.pitchRange = 0;
        s.yaw = 0;
        s.yawRange = 0;
    }
    if (Script_SeekToKeyword('c')) {
        s.verticalStart = Script_GetValue();
        s.verticalStartRange = Script_GetValue();
        s.verticalDecay = Script_GetValue();
        s.verticalDecayRange = Script_GetValue();
        s.horizontalStart = Script_GetValue();
        s.horizontalStartRange = Script_GetValue();
        s.horizontalDecay = Script_GetValue();
        s.horizontalDecayRange = Script_GetValue();
    } else {
        s.verticalStart = 255;
        s.verticalStartRange = 0;
        s.verticalDecay = 255;
        s.verticalDecayRange = 0;
        s.horizontalStart = 255;
        s.horizontalStartRange = 0;
        s.horizontalDecay = 255;
        s.horizontalDecayRange = 0;
    }
    ImpactBurst_Create(count, &pos, &s);
}
