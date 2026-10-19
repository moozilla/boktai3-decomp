#define SceneActorPlacement_ResumeActor sub_08136D40
#include "global.h"
#include "scene_actor_placement.h"
void SceneActorTerrain_ResumeDrawable(void *);
void ActorShadow_Enable(void *);
static inline void ClearCollider(struct ContactCollider *p) { p->flags = 0xFFFB & p->flags; }
static inline void ClearWord(u32 *p, u32 mask) { *p &= mask; }
void sub_08136D40(struct SceneActorPlacementView *s)
{
    u32 one = 1;
    if (s->flags2CC & one) {
        u8 zeroByte;
        u32 zeroWord;
        SceneActorTerrain_ResumeDrawable(s);
        if (s->shadow320[0]) ActorShadow_Enable(s->shadow320);
        ClearCollider(&((struct SceneActorRegistryResetView *)s)->colliderA);
        zeroByte = 0;
        zeroWord = 0;
        SceneActorRegistry_ResetRouteAndColliders((void *)s);
        s->state2A8 = 3;
        s->phase2A9 = zeroByte;
        s->behavior2AA = zeroByte;
        s->counter2AC = zeroWord;
        s->field2B1 = one;
        {
            const void *control = s->resetControl220;
            u8 code = 12;
            s->field2B2 = one;
            s->field2B0 = zeroByte;
            s->field2B5 = code;
            s->control = control;
        }
        s->field2C8 = zeroWord;
        ClearWord(&s->flags114, ~1);
        s->field104 = zeroWord;
    }
}
