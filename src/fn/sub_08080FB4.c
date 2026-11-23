#include "scene_actor_behavior.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "runtime.h"
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
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag(struct ChaseActor *actor, u32 maskInput, u32 off)
{
    /* The actor stores flags2D4 as a halfword and flags2D0 as a word. */
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline void SetControl(struct ChaseActor *actor)
{
    void *callback = (void *)SceneActorControl_OnIdle;
    u8 kind = 16;
    actor->base.field2B2 = 1;
    actor->base.stepDone = 0;
    actor->base.controlKind = kind;
    actor->base.target = callback;
    {
        /* Preserve the short-lived halfword-clear address. */
        {
            u16 *field = &actor->base.field2C8;
            *field = 0;
        }
    }
    {
        u32 *field = &actor->base.pendingControlFlags;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.pendingControl = 0;
}
static inline void SetAnimation(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
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
            struct SceneActorRegistryControl *control = &actor->group.common.control;
            u32 *field;
            u32 mask;
            field = &actor->base.behaviorFlags;
            mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
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
        u32 offset = 0;
        values[0] = *(s16 *)((u8 *)&actor->base.body.position + offset);
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        offset = 0x1FC;
        values[3] = *((s32 *)(((u8 *)actor) - (-offset)));
        values[4] = actor->base.parameters1FC[1];
        values[5] = actor->base.parameters1FC[2];
        offset += 0xE4;
        values[6] = *(s32 *)((u8 *)actor + offset);
        values[7] = actor->base.body.id;
        arguments.count = 8;
        arguments.values = values;
        /* Keep the payload offset live through argument construction. */
        Script_Call(script, &arguments);
    }
    if ((!Flag(actor, 0x4000, 0x2D4)) && (((u32)(actor->base.category - 11)) > 2)) {
        Bestiary_UnlockEntry(
            Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
    }
    actor->base.attachmentTimer = 0;
    SetControl(actor);
    SetAnimation(actor, 1, 0, 2);
    if ((!Test(gEngineFlags, 0x4800)) && (!Flag(actor, 0x800, 0x2D0))) {
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
    }
    ActorShadow_Disable((u8 *)actor + 0x320);
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
