#include "global.h"

void EffectGroup_FadeFirstActive(u8 *);

void SceneActorEffects_AdvanceSteps(u8 *p, s32 n)
{
    if (p[0x47c] == 1 && n > 0) {
        s32 i = n;
        do {
            EffectGroup_FadeFirstActive(p + 0x47c);
        } while (--i != 0);
    }
}
