// confidence=low: finishes the blink, runs defeat scripts and hides the drawable and shadow.
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "scene_actor_terrain.h"
#include "script_utility.h"
/* SceneActorControl_CompleteBlinkOut: keeps terrain timers and height updated while the drawable blinks (shadow follows visibility); when the blink expires runs the context and defeat scripts, sets the defeat event flag, goes idle unless flags2D4 & 0x4000, and hides the drawable and shadow (provisional name) */
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
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
static inline u8 DrawableVisible(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 1;
    if (!actor->drawableKind) {
        if (*(u32 *)actor->drawable & mask)
            goto visible;
        goto hidden;
    }
    if (*(u32 *)((u8 *)actor->drawable + 0x28) & mask)
    visible:
        return 1;
hidden:
    return 0;
}
void SceneActorControl_CompleteBlinkOut(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u16 *flags;
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30)) {
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
    } else if (DrawableVisible(actor))
        ActorShadow_Disable((u8 *)actor + 0x320);
    else
        ActorShadow_Enable((u8 *)actor + 0x320);
}
