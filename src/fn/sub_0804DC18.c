#include "staged_effect.h"
void Script_Call(u32, u32);
void StagedEffect_ReleaseScript(struct StagedEffect *effect)
{
    if (effect->frame == effect->scriptReleaseFrame) {
        if (effect->pendingScriptId) {
            Script_Call(effect->pendingScriptId, 0);
            effect->pendingScriptId = 0;
        }
    }
    effect->frame++;
}
