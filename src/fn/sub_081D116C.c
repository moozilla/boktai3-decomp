#include "global.h"
#include "ram.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetKeywordValue(s32, s32);
u8 *Actor_FindById(void *, s32);
void Actor_OnConfiguredAnimationUpdate(void);
void SceneNpc_SetBehaviorFourFromScript(void)
{
    u8 *p;
    if (!gSceneNpc)
        return;
    if (Script_SeekToKeyword(0x6e)) {
        p = gSceneActorListManager;
        if (!p)
            return;
        p = Actor_FindById(p, Script_GetKeywordValue(0x6e, 0));
        if (!p)
            return;
    } else
        p = gSceneNpc;
    {
        u8 *q = p + 0x21b;
        s32 zero = 0, one = 1;
        void (*f)(void);
        *q = one;
        {
            s32 kind = 6;
            p[2] = zero;
            p[3] = zero;
            p[4] = kind;
        }
        *(u32 *)(p + 0x14) = zero;
        p[8] = one;
        f = Actor_OnConfiguredAnimationUpdate;
        p[6] = 7;
        *(void (**)(void))(p + 0x2cc) = f;
        *(u32 *)(p + 0x10) = zero;
        p[9] = one;
        p[0x18] = zero;
    }
}
