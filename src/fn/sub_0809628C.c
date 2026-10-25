// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining difference: the script value array (values[]) stores go through a hoisted sp+0x24 base register instead of direct sp-relative stores; script/offset temporaries then shift registers.
#include "scene_actor_reaction.h"
#include "script_utility.h"
void SceneActorEffects_AdvanceSteps(void *, u32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void Bee_ConsumeFlag80(void *);
void SceneActor_EndLinkedReaction(void *);
void Sound_PlaySong(u32);
void EffectManager_SpawnParticlesAt(u32, u32, u32, void *, void *, u32, u32);
s32 Mod(s32, s32);
extern u32 gUnk_03005308;
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
// confidence=low: defeat control; emits debris sprites, runs the defeat script, then returns to idle.
void Ghost_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    u8 *ctx = actor->context;
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
            u32 index = (gUnk_03005308 + 1) & 1023;
            u32 count;
            gUnk_03005308 = index;
            count = Mod(*((u16 *)(0x0203B400 + (index * 2))), 3) + 1;
            pos = &actor->position;
            {
                u16 *a = (u16 *)pos;
                u16 *b = (u16 *)(((u8 *)actor) + 0x5E);
                u16 *c = (u16 *)(((u8 *)actor) + 0x60);
                px = *a;
                py = *b;
                pz = *c;
            }
            {
                register u16 *q asm("r1") = &pieces[0].x;
                register s32 n asm("r2") = 2;
                i = n;
                do {
                    q[0] = px;
                    q[1] = py;
                    q[2] = pz;
                    q += 4;
                } while ((--i) >= 0);
            }
            EffectManager_SpawnParticlesAt(count, 0, *((u8 *)(ctx + 0x5BC)), pos, pieces, 300, 120);
        }
    }
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    if (script) {
        s32 values[8];
        struct ScriptUtilityArguments arguments;
        values[0] = actor->position.x;
        {
            typeof(values[1]) *q1 = &values[1];
            *q1 = actor->position.y;
        }
        values[2] = actor->position.z;
        values[3] = *((s32 *)(((u8 *)actor) + 0x1FC));
        values[4] = *((s32 *)(((u8 *)actor) + 0x200));
        values[5] = *((s32 *)(((u8 *)actor) + 0x204));
        values[6] = *((s32 *)(((u8 *)actor) + 0x2E0));
        values[7] = actor->id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    script = *((u16 *)(((u8 *)actor) + 0x1F8));
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

#else
INCLUDE_ASM("asm/nonmatching", Ghost_UpdateDefeat);
#endif
