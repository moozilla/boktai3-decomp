#include "scene_actor_chase.h"
// confidence=low: animation completion resumes group movement or idle.
static inline u8 TakeEntry(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
void Dog_ResumeAfterReaction(struct ChaseActor *a)
{
    u8 *group;
    u16 flags;
    if (TakeEntry(a)) {
        SceneActorAnimation_SelectDirectionalEntry(a, 11);
        a->base.restart = 0;
    }
    if (!a->base.restart) {
        return;
    }
    group = (u8 *)&a->group.common;
    a->base.field2B0 = 1;
    {
        u32 mask = 0x80;
        flags = a->base.flags2D4;
        flags = a->base.flags2D4 & mask;
    }
    if (flags) {
        SceneActor_SetPositionXZ((u8 *)a + 0x50, *(s16 *)(*(u8 **)((u8 *)a + 0x5D0) + 0x1C),
                                 *(s16 *)(*(u8 **)((u8 *)a + 0x5D0) + 0x20));
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        Chase_SetCtl(a, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(a, 12);
        {
            u16 *field = (u16 *)((u8 *)a + 0x414);
            u32 mask = 4;
            *field = (*(&mask)) | (*field);
        }
        a->base.field2C2 = 0x20;
    } else {
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        Chase_SetCtl(a, SceneActorControl_OnIdle, 0x10);
        Chase_SetBehavior(a, 7);
        a->base.flags2CC |= 0x10;
    }
}
