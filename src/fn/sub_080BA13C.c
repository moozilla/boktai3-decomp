#define SceneActorReaction_TickQueuedContact sub_080BA13C
/* Tentative reaction name, based on the observed fields and calls. */
#include "global.h"
#include "scene_actor_reaction.h"
void sub_080BA268(void);
void Skeleton_UpdateAttackRecovery(void);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void sub_080BA13C(struct SceneActorRegistryResetView *p)
{
    u8 *state = p->context;
    u16 *animation = (u16 *)((u8 *)p->drawable + 0x20);
    u32 *flags;
    u32 queued;
    if (SceneActorReaction_TakePrepared(p))
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)p, 5);
    if (animation[10] == 2 && animation[6] == 0)
        Sound_PlaySong(0x3A9);
    if (animation[10] == 3) {
        Collider_Refresh(state + 0x678);
    }
    {
        u32 mask = 0x20;
        u32 *address = (u32 *)(state + 0x6DC);
        u32 value = *address & mask;
        flags = address;
        queued = Test(value, ~0);
    }
    if (queued) {
        {
            s32 mask = ~0x20;
            *flags &= mask;
        }
        {
            void (*f)(void) = sub_080BA268;
            u32 k = 0x25;
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
    } else if (p->field2B3) {
        void (*f)(void) = Skeleton_UpdateAttackRecovery;
        u32 k = 0x24;
        u32 z;
        u8 *a = &p->field2B2;
        u32 *w;
        s32 m;
        z = queued;
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
