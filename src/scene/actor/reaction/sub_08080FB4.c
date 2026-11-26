#include "scene/actor/scene_actor_behavior.h"
#include "scene/actor/scene_actor_reaction.h"
#include "script/script_utility.h"
#include "engine/runtime.h"
#include "scene/actor/scene_actor_control_helpers.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorRecord_RecordPair(u16, u16);
static inline u8 IsPrimary(struct ChaseActor *actor)
{
    if (actor->base.object == (void *)1)
        return 1;
    return 0;
}
static inline u32 Flag(struct ChaseActor *actor, u32 maskInput, u32 off)
{
    /* The actor stores flags2D4 as a halfword and flags2D0 as a word. */
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
void SceneActorReaction_BeginHiddenDefeat(struct ChaseActor *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    if (!SceneActorReaction_TakePrepared(actor)) {
        return;
    }
    if (IsPrimary(actor)) {
        {
            SceneActorReaction_ResetRouteControl(actor, &actor->group.common.control);
        }
        /* Frame127 releases attachments without consuming offset outputs. */
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
    }
    {
        u32 showFlag = 1;
        if (actor->base.drawableKind == 0) {
            u32 *sequence = actor->base.drawable;
            *sequence |= showFlag;
        } else {
            u8 *tile = (u8 *)actor->base.drawable + 0x20;
            *(u32 *)(tile + 8) |= showFlag;
        }
    }
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
    if ((!Flag(actor, 0x4000, 0x2D4)) && (((u32)(actor->base.category - 11)) > 2)) {
        Bestiary_UnlockEntry(
            Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    }
    actor->base.attachmentTimer = 0;
    SceneActorTerrain_InstallControl(actor, SceneActorControl_OnIdle, 16);
    SceneActorTerrain_EnterState(actor, 1, 0, 2);
    if ((!SceneActorControl_TestFlags(gEngineFlags, 0x4800)) && (!Flag(actor, 0x800, 0x2D0))) {
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    }
    ActorShadow_Disable(actor->base.shadow);
    switch (actor->base.category) {
        case 4:
            Sound_PlaySong(212);
            break;
        case 1:
            Sound_PlaySong(253);
            break;
        case 3:
            SceneActorPlacement_PlayVariantEventSound((void *)actor, 1);
            break;
        case 25:
            Sound_PlaySong(459);
            break;
        case 10:
            Sound_PlaySong(318);
            break;
        case 21:
            Sound_PlaySong(318);
            break;
    }
}
