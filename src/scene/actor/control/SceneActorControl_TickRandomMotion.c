#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "scene/actor/scene_actor_reaction.h"
#include "player/player.h"
#include "engine/math/math_shift.h"
extern const s16 gUnk_086149C4[];
void CpuSet(const void *, void *, u32);
void SceneActorControl_ResetCollisionPhase(struct ChaseActor *);
void SceneActorControl_StartPositionCurve(struct ChaseActor *, const struct ContactPosition *);
static inline void ClearMotionVector(u32 *zero, struct ContactPosition *out)
{
    *zero = 0;
    CpuSet(zero, out, 0x05000002);
}

void SceneActorControl_TickRandomMotion(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *control = SceneActorControl_GetMotionState(actor);
    u32 zero;

    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        ClearMotionVector(&zero, &control->vectors[2]);
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        control->randomMotionAngle = ((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
        actor->base.secondaryMotion.x =
            Math_ShiftTowardZero(gUnk_086149C4[control->randomMotionAngle], 7);
        actor->base.secondaryMotion.z =
            Math_ShiftTowardZero(gUnk_086149C4[(control->randomMotionAngle + 64) & 255], 7);
    }
    if ((s32)actor->base.counter2AC <= 5) {
        actor->base.secondaryMotion.x =
            Math_ShiftTowardZero(gUnk_086149C4[control->randomMotionAngle], 7);
        actor->base.secondaryMotion.z =
            Math_ShiftTowardZero(gUnk_086149C4[(control->randomMotionAngle + 64) & 255], 7);
    }
    {
        struct ContactPosition *motion = &actor->base.secondaryMotion;
        if (!(motion->x * motion->x + motion->z * motion->z)) {
            SceneActorControl_ResetCollisionPhase(actor);
            ClearMotionVector(&zero, motion);
            SceneActorControl_StartPositionCurve(
                actor, (const struct ContactPosition *)&gPlayers[0]->position);
            SceneActorTerrain_EnterState(actor, 0, 2, 0);
        }
    }
    ++actor->base.counter2AC;
}
