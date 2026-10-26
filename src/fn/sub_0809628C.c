// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void Bee_ConsumeFlag80(void *);
void Sound_PlaySong(u32);
void EffectManager_SpawnParticlesAt(u32, u32, u32, void *, void *, u32, u32);
s32 Mod(s32, s32);
struct GhostDefeatContext {
    u8 unknown0[0x5BC];
    u8 particleStyle;
};
struct GhostDefeatScriptView {
    u8 unknown0[0x1F8];
    u16 scriptId;
    u8 unknown1FA[2];
    s32 parameters[3];
};
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? actor->flags2D4 & mask : actor->flags2D0 & mask;
}
static inline void Ghost_SetScriptValue(s32 value, s32 *entry)
{
    *entry = value;
}
// confidence=low: defeat control; emits debris sprites, runs the defeat script, then returns to idle.
void Ghost_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    struct GhostDefeatContext *ctx = actor->context;
    struct {
        u16 x;
        u16 y;
        u16 z;
        u16 w;
    } pieces[3];
    u16 script;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        actor->colliderA.flags |= 4;
        if (Flag(actor, 8, 0x2D4))
            Sound_PlaySong(0x12A);
        Bee_ConsumeFlag80(actor);
        if (!Flag(actor, 0x4000, 0x2D4)) {
            s32 i;
            u16 px;
            u16 py;
            register u16 pz asm("r0");
            struct ContactPosition *pos;
            u32 index = (gRandomIndex + 1) & 1023;
            u32 count;
            gRandomIndex = index;
            count = Mod(*((u16 *)(0x0203B400 + (index * 2))), 3) + 1;
            pos = &actor->position;
            {
                u16 *a = (u16 *)pos;
                u16 *b = (u16 *)&actor->position.y;
                u16 *c = (u16 *)&actor->position.z;
                px = *a;
                py = *b;
                pz = *c;
            }
            {
                u16 *q = &pieces[0].x;
                s32 n = 2;
                i = n;
                do {
                    q[0] = px;
                    q[1] = py;
                    q[2] = pz;
                    q += sizeof(pieces[0]) / sizeof(*q);
                } while ((--i) >= 0);
            }
            EffectManager_SpawnParticlesAt(count, 0, ctx->particleStyle, pos, pieces, 300, 120);
        }
    }
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    script = ((struct GhostDefeatScriptView *)actor)->scriptId;
    if (script) {
        s32 values[8];
        struct ScriptUtilityArguments arguments;
        Ghost_SetScriptValue(actor->position.x, &values[0]);
        Ghost_SetScriptValue(actor->position.y, &values[1]);
        Ghost_SetScriptValue(actor->position.z, &values[2]);
        Ghost_SetScriptValue(((struct GhostDefeatScriptView *)actor)->parameters[0], &values[3]);
        Ghost_SetScriptValue(((struct GhostDefeatScriptView *)actor)->parameters[1], &values[4]);
        Ghost_SetScriptValue(((struct GhostDefeatScriptView *)actor)->parameters[2], &values[5]);
        Ghost_SetScriptValue(actor->field2E0, &values[6]);
        Ghost_SetScriptValue(actor->id, &values[7]);
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        u16 *flags;
        u32 zero;
        u32 one;
        u16 masked;
        u32 test = actor->flags2D4 & mask;
        flags = &actor->flags2D4;
        if ((!test) && (((u32)(actor->category - 11)) > 2))
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        {
            u32 mask2 = 0x4000;
            u32 value = *flags;
            masked = value & mask2;
        }
        if (!masked) {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            one = (actor->field2B2 = 1);
            zero = 0;
            actor->field2B0 = masked;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = masked;
            {
                u32 *field = &actor->flags114;
                s32 clear = ~1;
                *field &= clear;
            }
            actor->word104 = masked;
            {
                u32 behavior = 2;
                actor->state2A8 = one;
                actor->phase2A9 = zero;
                actor->behavior2AA = behavior;
                actor->counter2AC = masked;
                actor->field2B1 = one;
            }
        }
    }
}
