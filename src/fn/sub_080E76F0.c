#include "global.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void SparkParticle_Spawn(void *, u32, void *, s32, s32, s32, s32, s32);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
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
static inline void SetControl(struct SceneActorRegistryResetView *actor, u32 kind, void *target)
{
    void *callback = target;
    u32 type = kind;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = type;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline void ResetElapsed(struct DisplaySequenceState *sequence)
{
    sequence->elapsed = 0;
}
// confidence=low: Mimic defeat blink inferred from neighboring behavior callbacks.
void Mimic_UpdateDefeatBlink(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ContactPackedVector range;
    struct ScriptUtilityArguments arguments;
    if (SceneActorReaction_TakePrepared(actor))
        SceneActorAnimation_SelectEntryFlags((void *)actor, 10,
                                             SceneActor_GetDirectionFlags(actor->body.direction));
    ResetElapsed((void *)((u8 *)actor->drawable + 0x48));
    if (frame <= 19)
        return;
    if (!(frame & 1)) {
        range.z = 128;
        range.x = 128;
        range.y = 64;
        SparkParticle_Spawn(&actor->body.position, 1, &range, 0, 0, 0, 2, 2);
        if (!(frame & 3))
            Sound_PlaySong(0x16E);
    }
    if (Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30)) {
        u16 script;
        SetControl(actor, 16, SceneActorControl_OnIdle);
        SetState(actor, 1, 0, 2);
        *(u32 *)actor->drawable |= 1;
        ActorShadow_Disable((u8 *)actor + 0x320);
        SceneActorBehavior_CallTerrainScript(actor);
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
            u32 m = 0x4000;
            if (!(actor->flags2D4 & m) && (u32)(actor->category - 11) > 2)
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        }
    } else if (DrawableVisible(actor))
        ActorShadow_Disable((u8 *)actor + 0x320);
    else
        ActorShadow_Enable((u8 *)actor + 0x320);
}
