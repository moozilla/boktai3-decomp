#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(struct SceneActorBehaviorView *);
void SceneActorPlacement_PlayCategoryEventSound(struct SceneActorRegistryActor *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
static inline void SetAnimation(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                                u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
extern const u16 gUnk_0203B400[];
static inline u8 Hidden(struct SceneActorRegistryResetView *a)
{
    u32 mask = 1;
    if (!a->drawableKind) {
        if (*(u32 *)a->drawable & mask)
            goto yes;
        else
            goto no;
    } else {
        if (*(u32 *)((u8 *)a->drawable + 0x28) & mask)
            goto yes;
        else
            goto no;
    }
yes:
    return 1;
no:
    return 0;
}
void Spider_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u32 *stateFlags;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        *(u16 *)((u8 *)actor + 0x2BC) = 60;
        {
            u32 index = (gRandomIndex + 1) & 1023;
            u32 value;
            gRandomIndex = index;
            index *= 2;
            {
                const u16 *table = (const u16 *)0x0203B400;
                u32 off;
                {
                    const u16 *entry = (void *)(index + (u32)table);
                    value = *entry;
                }
                off = 0x2BA;
                {
                    u8 *field = (u8 *)actor + off;
                    *field = value;
                }
            }
        }
        {
            u32 off = 0x2BB;
            u8 *p;
            p = (u8 *)actor + off;
            *p = 1;
        }
    }
    {
        u32 mask = 0x8000;
        u32 off = 0x2CC;
        u32 *p;
        u32 v;
        p = (u32 *)((u8 *)actor + off);
        v = *p & mask;
        stateFlags = p;
        if (v) {
            return;
        }
    }

    if (Sprite_BlinkUntilExpired(actor->drawable, frame, 10, 30)) {
        {
            u32 off = 0x2B0;
            u8 *q;
            q = (u8 *)actor + off;
            *q = 1;
        }
        {
            u32 off = 0x1F8;
            u16 *q;
            q = (u16 *)((u8 *)actor + off);
            script = *q;
        }
        if (script) {
            values[0] = actor->position.x;
            values[1] = actor->position.y;
            values[2] = actor->position.z;
            values[3] = *(s32 *)((u8 *)actor + 0x1FC);
            values[4] = *(s32 *)((u8 *)actor + 0x200);
            values[5] = *(s32 *)((u8 *)actor + 0x204);
            values[6] = actor->field2E0;
            values[7] = actor->id;
            arguments.count = 8;
            arguments.values = values;
            Script_Call(script, &arguments);
        }
        {
            u32 mask = 0x4000;
            if ((!(actor->flags2D4 & mask)) && (((u32)(actor->category - 11)) > 2)) {
                GameState_SetEventFlag(
                    BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
            }
        }
        {
            u32 mask = 0x400;
            if (!((*stateFlags) & mask)) {
                SceneActorBehavior_CallTerrainScript((void *)actor);
            }
        }
        {
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
        }
        SetAnimation(actor, 1, 0, 2);
    } else if (Hidden(actor)) {
        ActorShadow_Disable(((u8 *)actor) + 0x320);
    } else {
        ActorShadow_Enable((u8 *)actor + 0x320);
    }
}
