#include "scene_actor_behavior.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);
void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
void SceneActorControl_CompleteScriptedResponse(struct SceneActorRegistryResetView *actor,
                                                s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            u32 offset = 0x134;
            struct ContactCollider *collider = (void *)((u8 *)actor + offset);
            collider->flags |= 4;
        }
    }
    if (!Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30))
        return;
    actor->base.field2B0 = 1;
    script = (u16)((struct SceneActorBehaviorView *)actor)->script1F8;
    if (script) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[0];
        values[4] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[1];
        values[5] = ((struct SceneActorBehaviorView *)actor)->parameters1FC[2];
        values[6] = actor->base.field2E0;
        values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        if (!(actor->base.flags2D4 & mask) && (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    }
    {
        u32 mask = 0x400;
        if (!(actor->base.flags2CC & mask))
            SceneActorBehavior_CallTerrainScript((void *)actor);
    }
    actor->base.fieldAA = 0;
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind;
        kind = 16;
        actor->base.field2B2 = 1;
        actor->base.field2B0 = 0;
        actor->base.field2B5 = kind;
        actor->base.target = callback;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.flags114;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.word104 = 0;
    }
    SetAnimation(actor, 1, 0, 2);
    ActorShadow_Disable((u8 *)actor + 0x320);
}
