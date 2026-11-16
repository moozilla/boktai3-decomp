#include "global.h"
#include "axe.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_behavior.h"
#include "scene_actor_placement.h"
#include "scene_actor_chase_shared.h"

void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *, void *);
void ActorPhase_UpdateReverseFlicker(void);

static inline u16 Axe_ContactResponse(const struct SceneActorBehaviorContactView *contact)
{
    return contact->parameter44;
}
static inline s32 Axe_ReadContactIndex(const s32 *index)
{
    return *index;
}
static inline s32 Axe_ContactIndex(s32 index)
{
    return Axe_ReadContactIndex(&index);
}
// Records the first contact response, then disarms the active collider banks
// and enters reverse flicker when the armed contact timer exceeds two.
void Axe_OnBodyContact(struct SceneActorBehaviorContactView *source,
                       struct SceneActorBehaviorContactView *destination, void *object)
{
    struct ChaseActor *actor = object;
    struct Axe *context = actor->base.context;
    if (SceneActorPlacement_IsUnavailable((void *)actor))
        return;
    SceneActorBehavior_ApplyContactReaction(source, destination, actor);
    if (actor->base.colliderA.responseValue == 0) {
        actor->base.colliderA.cost = destination->remaining;
        actor->base.field2E6 = destination->mode;
        actor->base.colliderA.responseValue = Axe_ContactResponse(destination);
    }
    if ((context->flagsCAB & 2) && context->timerEB0 > 2) {
        s32 i;
        u32 stride = sizeof(struct ContactCollider);
        i = 0;
        do {
            u32 attackStride = sizeof(context->attack[0]);
            struct ContactCollider *attackBase = context->attack;
            struct ContactCollider *attack = (void *)((u8 *)attackBase + i * attackStride);
            /* Capture the body bank record layout at this iteration. */
            typedef u8 AxeBodyRecord[stride];
            struct ContactCollider *body =
                (struct ContactCollider *)&((AxeBodyRecord *)context->body)[i];
            attack->flags |= 4;
            body->flags |= 4;
            ++i;
        } while (Axe_ContactIndex(i) <= 2);
        SceneActorChase_SetControl(actor, (void *)ActorPhase_UpdateReverseFlicker, 0x2F);
        context->timerEB0 = 0;
    }
}
