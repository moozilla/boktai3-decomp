#define SceneActorReaction_TickContactAnimation sub_080BA010
/* Tentative reaction name, based on the observed fields and calls. */
#include "global.h"
#include "scene_actor_reaction.h"
void sub_080BA0EC(void);
void sub_080BA010(struct SceneActorRegistryResetView *p)
{
    u8 *state = p->context;
    u16 *animation = (u16 *)((u8 *)p->drawable + 0x20);
    u8 *category;
    u32 kind;
    if (SceneActorReaction_TakePrepared(p))
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 5);
    kind = p->category;
    category = &p->category;
    if (kind == 5 && animation[10] == 2 && animation[6] == 0)
        Sound_PlaySong(0x3A9);
    if (animation[10] == 3) {
        Collider_Refresh(state + 0x678);
        if (animation[6] == 0 && *category == 6)
            Sound_PlaySong(0x3AA);
    }
    if (p->field2B3) {
        void (*f)(void) = sub_080BA0EC;
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
