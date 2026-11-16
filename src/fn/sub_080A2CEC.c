#include "scene_actor_registry.h"
#include "scene_actor_behavior.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "ram.h"
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorBehavior_CallTerrainScript(struct RoomActor *);
void SceneActorPlacement_PlayCategoryEventSound(struct ChaseActor *);
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
static inline void SetAnimation(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
static inline u8 Hidden(struct ChaseActor *a)
{
    u32 mask = 1;
    if (!a->base.drawableKind) {
        if (*(u32 *)a->base.drawable & mask)
            goto yes;
        else
            goto no;
    } else {
        if (((struct SceneActorRegistryTileDrawable *)a->base.drawable)->flags & mask)
            goto yes;
        else
            goto no;
    }
yes:
    return 1;
no:
    return 0;
}
void Spider_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    u32 *stateFlags;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        SceneActorPlacement_PlayCategoryEventSound((void *)actor);
        actor->base.defeatTimer = 60;
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

    if (Sprite_BlinkUntilExpired(actor->base.drawable, frame, 10, 30)) {
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
            if ((!(actor->base.motionFlags & mask)) && (((u32)(actor->base.category - 11)) > 2)) {
                Bestiary_UnlockEntry(
                    Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
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
        SetAnimation(actor, 1, 0, 2);
    } else if (Hidden(actor)) {
        ActorShadow_Disable(((u8 *)actor) + 0x320);
    } else {
        ActorShadow_Enable((u8 *)actor + 0x320);
    }
}
