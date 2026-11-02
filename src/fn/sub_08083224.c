#include "scene_actor_terrain_reaction.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
extern const s16 gUnk_086149C4[];
void SceneActorControl_UpdateShadowReturn(struct SceneActorRegistryResetView *);
void SceneActorControl_RestartPositionCurve(struct SceneActorRegistryResetView *);

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

void SceneActorControl_UpdateShadowReturnPhase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        SceneActorTerrain_InstallControl(actor, (void *)SceneActorControl_UpdateShadowReturn, 35);
        {
            u32 *flags = &actor->flags2D0;
            u32 mask = ~0x100000;
            *flags &= mask;
        }
        SetPolarMotion(&actor->secondaryMotion, actor->body.direction, 4);
        actor->secondaryMotion.y = 10;
    }
    if (actor->field2B0 && (s16)actor->field2EA < control->returnHeight - 20)
        SceneActorControl_RestartPositionCurve(actor);
    else
        ++actor->counter2AC;
}
