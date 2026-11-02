#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_control_motion.h"
#include "scene_actor_reaction.h"
#include "player.h"
extern const s16 gUnk_086149C4[];
void CpuSet(const void *, void *, u32);
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *);
void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *,
                                          const struct ContactPosition *);
static inline s32 ScaleMotionSample(s32 v)
{
    return v >= 0 ? v >> 7 : -(-v >> 7);
}
static inline void ClearMotionVector(u32 *zero, struct ContactPosition *out)
{
    *zero = 0;
    CpuSet(zero, out, 0x05000002);
}

void SceneActorControl_TickRandomMotion(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor = input;
    struct SceneActorControlMotionState *control = SceneActorControl_GetMotionState(actor);
    u32 zero;
    u32 magnitude;
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        u32 index;
        SceneActorTerrain_InstallControl(actor, actor->entryTarget228, 1);
        ClearMotionVector(&zero, &control->vectors[2]);
        index = (gRandomIndex + 1) & 1023;
        gRandomIndex = index;
        {
            const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
            u32 value;
            value = table[index];
            {
                u8 *angle = &control->randomMotionAngle;
                *angle = value;
                actor->secondaryMotion.x = ScaleMotionSample(gUnk_086149C4[*angle]);
            }
        }
        {
            const s16 *sine = gUnk_086149C4;
            u8 *field = &control->randomMotionAngle;
            u32 angle = *field + 64;
            u32 mask = 255;
            s32 sample;
            angle &= mask;
            sample = sine[angle];
            actor->secondaryMotion.z = ScaleMotionSample(sample);
        }
    }
    if ((s32)actor->counter2AC <= 5) {
        const s16 *first = gUnk_086149C4;
        u8 *field;
        const s16 *table;
        s32 value;

        field = &control->randomMotionAngle;
        value = first[*field];
        table = first;
        actor->secondaryMotion.x = ScaleMotionSample(value);

        field = &control->randomMotionAngle;
        value = table[(*field + 64) & 255];
        actor->secondaryMotion.z = ScaleMotionSample(value);
    }
    {
        struct ContactPosition *motion = &actor->secondaryMotion;
        s32 x = motion->x;
        u32 squareX = (u32)x * x;
        {
            s32 z = motion->z;
            u32 squareZ = (u32)z * z;
            {
                u32 value = squareZ;
                magnitude = squareX + value;
            }
        }
        if (!magnitude) {
            SceneActorControl_ResetCollisionPhase(actor);
            ClearMotionVector(&zero, motion);
            SceneActorControl_StartPositionCurve(
                actor, (const struct ContactPosition *)&gPlayers[0]->position);
            SceneActorTerrain_EnterState(actor, 0, 2, 0);
        }
    }
    ++actor->counter2AC;
}
