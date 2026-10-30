// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
extern const s16 gUnk_086149C4[];
void SceneActorControl_UpdateShadowReturn(struct SceneActorRegistryResetView *);
void SceneActorControl_RestartPositionCurve(struct SceneActorRegistryResetView *);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
void SceneActorControl_UpdateShadowReturnPhase(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlState *control = SceneActorControl_GetState(actor);
    if (SceneActorControl_TakeFlag(&actor->field2B1)) {
        void *callback = (void *)SceneActorControl_UpdateShadowReturn;
        u32 kind;
        kind = 35;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;

        {
            u32 *flags = &actor->flags2D0;
            u32 mask = ~0x100000;
            *flags &= mask;
        }
        {
            u8 *heading = &actor->body.direction;
            struct ContactPosition *motion = &actor->secondaryMotion;
            u32 direction;
            register const s16 *table asm("r2");
            direction = *heading;
            table = gUnk_086149C4;
            motion->x = Scale(table[(direction + 64) & 255] * 4);
            motion->y = 0;
            {
                const s16 *entry = (void *)(direction * 2 + (u32)table);
                s32 value;
                value = *entry;
                {
                    s32 factor = 4;
                    asm("" : "+r"(factor) : "r"(value));
                    motion->z = Scale(factor * value);
                }
            }
        }
        actor->secondaryMotion.y = 10;
    }
    if (actor->field2B0 && (s16)actor->field2EA < *(u16 *)((u8 *)control + 0x3D8) - 20)
        SceneActorControl_RestartPositionCurve(actor);
    else
        ++actor->counter2AC;
}
