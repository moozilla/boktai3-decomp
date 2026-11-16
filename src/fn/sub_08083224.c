#include "scene_actor_terrain_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
extern const s16 gUnk_086149C4[];
void SceneActorControl_UpdateShadowReturn(struct ChaseActor *);
void SceneActorControl_RestartPositionCurve(struct ChaseActor *);

static inline s32 ScaleMotionComponent(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}

static inline void SetPolarMotion(struct ContactPosition *m, u32 angle, s32 radius)
{
    m->x = ScaleMotionComponent(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius);
    m->y = 0;
    m->z = ScaleMotionComponent(gUnk_086149C4[angle] * radius);
}

void SceneActorControl_UpdateShadowReturnPhase(struct ChaseActor *actor)
{
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, (void *)SceneActorControl_UpdateShadowReturn, 35);
        {
            u32 *flags = &actor->base.flags2D0;
            u32 mask = ~0x100000;
            *flags &= mask;
        }
        SetPolarMotion(&actor->base.secondaryMotion, actor->base.body.direction, 4);
        actor->base.secondaryMotion.y = 10;
    }
    if (actor->base.field2B0 && (s16)actor->base.field2EA < control->returnHeight - 20)
        SceneActorControl_RestartPositionCurve(actor);
    else
        ++actor->base.counter2AC;
}
