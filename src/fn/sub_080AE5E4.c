#include "global.h"
#include "scene_actor_reaction.h"
extern const u8 gUnk_0824E20C[];
u8 Vorn_HasActiveResponse(void *);
void sub_080B3110(void *, u32, const u8 *, u32);
static inline void Resume_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                     void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void Resume_ResetAnimation(struct SceneActorRegistryResetView *actor, u32 state,
                                         u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
struct SceneActorResumeContext {
    u8 unknown0[0x6D4];
    u16 flags;
};
void SceneActorReaction_ResumeOrRunScript(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
    if (actor->field2B3) {
        actor->field2B0 = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        if (Vorn_HasActiveResponse(actor->context)) {
            struct SceneActorResumeContext *context = actor->context;
            u16 *flags = &context->flags;
            s32 mask = ~3;
            *flags &= mask;
            sub_080B3110(actor, 1, gUnk_0824E20C, 0x367);
        } else {
            {
                u32 mask = 16;
                actor->flags2CC |= mask;
            }
            Resume_ResetAnimation(actor, 3, 1, 0);
            {
                void *control = *(void **)(group + 0x210);
                Resume_SetControl(actor, 15, control);
            }
        }
    }
}
