#include "staged_effect.h"
#define StagedEffect_ReleaseScript sub_0804DC18
void Script_Call(void *, u32);
void StagedEffect_ReleaseScript(struct StagedEffect *effect)
{
    if (effect->frame == effect->scriptReleaseFrame) {
        if (effect->pendingScript) {
            Script_Call(effect->pendingScript, 0);
            effect->pendingScript = 0;
        }
    }
    effect->frame++;
}
