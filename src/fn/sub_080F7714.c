#define SceneActor_ConfigureTrailSegments Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "scene_actor_reaction.h"
struct SceneActorApproachContext {
    u8 unknown0[0x888], trail[0x19C];
    struct ContactPosition current;
    u8 unknownA2C[8], motionMode, unknownA35[0x253];
    struct ContactPosition destination;
    u8 unknownC90[0x1B], flagsCAB, unknownCAC[0x195], duration;
    u8 unknownE42[0x6D], remaining, unknownEB0[0x22], stateED2;
};
void Axe_SetAuxiliarySpriteRenderState(struct SceneActorRegistryResetView *, void *, u32);
void ActorPhase_EmitTrailSprite(struct SceneActorRegistryResetView *, struct ContactPosition *,
                                u32);
void Axe_UpdateTrailColliders(struct SceneActorRegistryResetView *);
void ActorPhase_UpdateTimedFlicker(struct SceneActorRegistryResetView *);
s32 Div(s32, s32);
static inline u8 SceneActor_TakeApproachEntry(struct SceneActorRegistryResetView *p)
{
    if (p->field2B2) {
        p->field2B2 = 0;
        return 1;
    }
    return 0;
}
static inline void SceneActor_SubtractApproachPosition(struct ContactPosition *out,
                                                       struct ContactPosition *a,
                                                       struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline void SceneActor_SetApproachControl(struct SceneActorRegistryResetView *p,
                                                 void *control, u32 kind)
{
    p->field2B2 = 1;
    p->field2B0 = 0;
    p->field2B5 = kind;
    p->target = control;
    p->field2C8 = 0;
    {
        u32 *flags = &p->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    p->word104 = 0;
}
void SceneActor_AdvanceOffsetApproach(struct SceneActorRegistryResetView *p)
{
    struct SceneActorApproachContext *context = p->context;
    struct ContactPosition difference;
    struct ContactPosition *current, *destination;
    if (SceneActor_TakeApproachEntry(p)) {
        context->stateED2 = 0;
        context->destination = p->body.position;
        context->destination.x -= 256;
        context->current = p->body.position;
        context->motionMode = 0;
        context->remaining = context->duration;
        context->flagsCAB = 0;
        Axe_SetAuxiliarySpriteRenderState(p, context->trail, 316);
    }
    current = &context->current;
    ActorPhase_EmitTrailSprite(p, current, context->motionMode);
    Axe_UpdateTrailColliders(p);
    SceneActor_SubtractApproachPosition(&difference, &context->destination, current);
    current->x += Div(difference.x, context->remaining);
    context->remaining--;
    if (!context->remaining) {
        SceneActor_SetApproachControl(p, ActorPhase_UpdateTimedFlicker, 43);
    }
}
