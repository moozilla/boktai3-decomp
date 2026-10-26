#include "global.h"
#include "ram.h"
void Effect_Remove(void *);
void EffectGroup_Unlink(void *, void *);
u32 SceneActorEffects_DestroyGroup(u8 *s)
{
    u8 *p = s + 0x1c;
    s32 i = 3;
    do {
        Effect_Remove(p);
        p += 0x48;
        i--;
    } while (i >= 0);
    if (gSceneActorEffectsManager)
        EffectGroup_Unlink(gSceneActorEffectsManager, s);
    return 0;
}
