// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_attachment.h"

// confidence=low: preserves the existing tentative reaction-state interpretation.
#define SceneActorReaction_UpdateRouteRecovery sub_081301C4

#ifdef NONMATCHING
// fuzzy: 88.6%
// Remaining differences: state/control offset reuse, vector-store allocation, and packet zeros.
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 s, u8 p, u8 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
// confidence=low: timed movement recovery restores a route and saves a delayed control.
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void sub_0807C72C(void *, u32, u32, u32);
static inline void SaveControl(u8 *packet, void *control, u8 behavior)
{
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = behavior;
}
void SceneActorReaction_UpdateRouteRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u8 result;
    u32 *savedFlags;
    Chase_TakeArrived(actor);
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if ((s32)actor->counter2AC > 60) {
        struct ContactPosition target;
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x100;
            *flags &= mask;
        }
        sub_0807C72C(actor, 0, 2, actor->category);
        SetState(actor, 3, 1, 0);
        SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = 0x10;
            *flags |= mask;
        }
        {
            struct ContactPosition *p = &target;
            u16 *route = *(u16 **)(group + 0x7c);
            p->x = route[0];
            p->z = route[1];
            savedFlags = &actor->flags2CC;
            asm("" : "+r"(savedFlags));
            p->y = Chase_Height(p);
        }
        target.y = actor->position.y;
        Chase_TryRoute(actor, *(void **)(group + 0x1fc), &actor->position, &target);
        (*(struct ContactPosition **)(group + 0x200))->pad = 1;
        if (!(u8)SceneActorAttachment_TestPositionWindow(actor, &target, 32, 32)) {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xc0;
                *flags &= mask;
            }
            SaveControl(group + 0x8c, actor->entryTarget228, 7);
        } else {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xd0;
                *flags &= mask;
            }
            SaveControl(group + 0x8c, actor->savedTarget, 0);
        }
        actor->field2C2 = 64;
    }
    actor->counter2AC++;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081301C4);
#endif
