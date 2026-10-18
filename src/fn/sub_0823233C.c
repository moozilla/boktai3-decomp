#include "global.h"
extern void *gUnk_0200005C;
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
    if (gUnk_0200005C)
        EffectGroup_Unlink(gUnk_0200005C, s);
    return 0;
}
