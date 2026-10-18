/* Tentative reaction name, based on the observed fields and calls. */
#include "global.h"
#include "scene_actor_reaction.h"
void SceneActorReaction_ApplyThreshold(void);
void SceneActorReaction_TickMultiHitAnimation(struct SceneActorRegistryResetView *p)
{
    u8 *state = p->context;
    u16 *animation = (u16 *)((u8 *)p->drawable + 0x20);
    u32 frame;
    if (SceneActorReaction_TakePrepared(p))
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 0x17);
    frame = animation[10];
    if ((u16)(frame - 3) <= 2 || (u16)(frame - 7) <= 2)
        Collider_Refresh(state + 0x678);
    if ((animation[10] == 4 || animation[10] == 7) && animation[6] == 0)
        Sound_PlaySong(0x3AA);
    if (p->field2B3) {
        void (*f)(void) = SceneActorReaction_ApplyThreshold;
        u32 k = 0x3e;
        u32 z;
        u8 *a = &p->field2B2;
        u32 *w;
        s32 m;
        z = 0;
        *a = 1;
        p->field2B0 = z;
        p->field2B5 = k;
        p->target = f;
        p->field2C8 = z;
        w = &p->flags114;
        m = -2;
        *w = m & *w;
        p->word104 = z;
    }
}
