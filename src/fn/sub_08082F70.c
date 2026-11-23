#include "scene_actor_behavior.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "scene_actor_terrain_reaction.h"
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorBehavior_CallTerrainScript(struct RoomActor *);
void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void SceneActorControl_CompleteScriptedResponse(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            collider->flags |= 4;
        }
    }
    if (!Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30))
        return;
    actor->base.stepDone = 1;
    script = (u16)actor->base.script1F8;
    if (script) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = actor->base.parameters1FC[0];
        values[4] = actor->base.parameters1FC[1];
        values[5] = actor->base.parameters1FC[2];
        values[6] = actor->base.hitOwners;
        values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        if (!(actor->base.motionFlags & mask) && (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    }
    {
        u32 mask = 0x400;
        if (!(actor->base.behaviorFlags & mask))
            SceneActorBehavior_CallTerrainScript((void *)actor);
    }
    actor->base.attachmentTimer = 0;
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind;
        kind = 16;
        actor->base.field2B2 = 1;
        actor->base.stepDone = 0;
        actor->base.controlKind = kind;
        actor->base.target = callback;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = 0;
    }
    SceneActorTerrain_EnterState(actor, 1, 0, 2);
    ActorShadow_Disable((u8 *)actor + 0x320);
}
