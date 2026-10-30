#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "script_utility.h"
/* SceneActorControl_BeginHiddenDefeat: on entry faces forward, releases attachments, plays the category sound and clears the defeat flags; once field2B3 is set either completes the defeat at once (flags2CC & 0x400: scripts, event flag, idle, hide) or hands over to the blink-out (SceneActorControl_CompleteBlinkOut, kind 0x27) (provisional name) */
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);
void SceneActorControl_CompleteBlinkOut(struct SceneActorRegistryResetView *, s32);
void SceneActorMovement_UpdateAirborne(struct SceneActorRegistryResetView *);
void SceneActorMovement_UpdateHeight(struct SceneActorRegistryResetView *);
void SceneActorControl_CallContextScript(struct SceneActorRegistryResetView *);
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void Hide(u8 *sprite, u32 mask)
{
    *(u32 *)(sprite + 8) |= mask;
}
void SceneActorControl_BeginHiddenDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u16 *flags;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        {
            struct SceneActorRegistryControl *control = &actor->group.control;
            u32 fieldOffset = 0x2CC;
            u32 *field;
            u32 mask;
            u32 zero;
            field = (void *)((u8 *)actor + fieldOffset);
            mask = ~0x141C0;
            *field &= mask;
            zero = 0;
            control->counter = zero;
            *(u16 *)((u8 *)control + 0x14) = zero;
            control->timer = zero;
        }
        {
            u16 *colliderFlags = (u16 *)((u8 *)actor + 0x13A);
            u32 mask;
            u32 value = *colliderFlags;
            mask = 4;
            mask |= value;
            *colliderFlags = mask;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (!actor->field2B3)
        return;
    {
        u32 mask = 0x400;
        u32 instant = actor->flags2CC & mask;
        if (instant) {
            actor->field2B0 = 1;
            SceneActorControl_CallContextScript(actor);
            script = *(u16 *)((u8 *)actor + 0x1F8);
            if (script) {
                values[0] = actor->body.position.x;
                values[1] = actor->body.position.y;
                values[2] = actor->body.position.z;
                values[3] = *(s32 *)((u8 *)actor + 0x1FC);
                values[4] = *(s32 *)((u8 *)actor + 0x200);
                values[5] = *(s32 *)((u8 *)actor + 0x204);
                values[6] = actor->field2E0;
                values[7] = actor->body.id;
                arguments.count = 8;
                arguments.values = values;
                Script_Call(script, &arguments);
            }
            {
                u32 mask = 0x4000;
                u16 value = actor->flags2D4;
                u16 test = value & mask;
                flags = &actor->flags2D4;
                if (!test && (u32)(actor->category - 11) > 2)
                    GameState_SetEventFlag(
                        BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
            }
            {
                u32 mask = 0x4000;
                u16 value = *flags;
                u16 busy = value & mask;
                if (!busy) {
                    void *callback = (void *)SceneActorControl_OnIdle;
                    u32 kind;
                    kind = 16;
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
                    SetAnimation(actor, 1, 0, 2);
                }
            }
            ActorShadow_Disable((u8 *)actor + 0x320);
            {
                u32 mask = 1;
                if (!actor->drawableKind)
                    *(u32 *)actor->drawable |= mask;
                else
                    Hide((u8 *)actor->drawable + 0x20, mask);
            }
        } else {
            void *callback = (void *)SceneActorControl_CompleteBlinkOut;
            u32 kind;
            kind = 0x27;
            actor->field2B2 = 1;
            actor->field2B0 = instant;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = instant;
            {
                u32 *flags = &actor->flags114;
                u32 mask = ~1;
                *flags &= mask;
            }
            actor->word104 = instant;
        }
    }
}
