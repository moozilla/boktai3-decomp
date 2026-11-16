/* Tentative name based on the observed state transition or initialization. */
#include "global.h"
#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalFlags(void *, s32);
void Sound_PlaySong(u32);
static inline u8 TakePrepared(struct ChaseActor *actor)
{
    if (actor->base.field2B2) {
        actor->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
void SceneActorReaction_PlayHiddenCue(struct ChaseActor *actor)
{
    if (TakePrepared(actor)) {
        u8 *drawable = actor->base.drawable;
        u32 zero = 0;
        drawable[7] = 2;
        actor->base.colliderA.responseValue = zero;
        if (!actor->base.drawableKind) {
            u32 *flags = actor->base.drawable;
            u32 clear = ~1;
            *flags &= clear;
        } else {
            u8 *tile = (u8 *)actor->base.drawable + 0x20;
            u32 clear = ~1;
            *(u32 *)(tile + 8) &= clear;
        }
        SceneActorAnimation_SelectDirectionalFlags(actor, 15);
        Sound_PlaySong(475);
    }
    if (actor->base.restart)
        actor->base.stepDone = 1;
}
