// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "script_utility.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: tentative Vorn behavior role.
// confidence=low: tentative Vorn behavior role recovered from the surrounding callbacks.
// Remaining differences: register choices, address derivation, and two late constant reloads.
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActor_CallContextScriptB(void *);
void Vorn_SendMessage(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
extern u32 gUnk_03005308;
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline u32 Random4(void)
{
    register u32 *cursor asm("r3") = &gUnk_03005308;
    u32 index = (*cursor + 1) & 1023;
    register u32 v asm("r1");
    *cursor = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)gUnk_0203B400);
    v &= 3;
    return v + 1;
}
/* The first spark's index arithmetic; the burst below passes the table explicitly. */
static inline u32 RandomFrom(const u16 *table)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    u32 v;
    gUnk_03005308 = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)table);
    v &= 3;
    return v + 1;
}
inline static u32 Random4_UpdateDefeat(void)
{
    register u32 *cursor asm("r3") = &gUnk_03005308;
    u32 index = ((*cursor) + 1) & 1023;
    register u32 v asm("r1");
    *cursor = index;
    index *= 2;
    v = *((const u16 *)(index + ((u32)(*((typeof(gUnk_0203B400) *)0x0203B400)))));
    v &= 3;
    return v + 1;
}

void Vorn_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    u16 script;
    u32 t1;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        SceneActorPlacement_PlayCategoryEventSound(actor);
        if (Flag(actor, t1 = 8, 0x2D4))
            Sound_PlaySong(0x12A);
        {
            struct SceneActorRegistryControl *control = &actor->group.control;
            u32 *field = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            *((u16 *)(((u8 *)control) + 0x14)) = 0;
            control->timer = 0;
        }
        {
            struct ContactCollider *collider = &actor->colliderA;
            collider->flags |= 4;
        }
        Vorn_SendMessage(actor);
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    if (!Flag(actor, 0x400, 0x2CC)) {
        u8 category;
        SceneActor_CallContextScriptB(actor);
        rangeB.x = 80;
        rangeB.y = 60;
        rangeB.z = t1 + 52;
        {
            register const u16 *table asm("r4") = *((typeof(gUnk_0203B400) *)0x0203B400);
            register s32 duration asm("r8");
            register s32 scale asm("sl");
            register s32 count asm("r9");
            register struct ContactPackedVector *range asm("r5") = &rangeB;
            register struct ContactPosition *position asm("r6");
            SparkParticle_Spawn(position = &actor->position, Random4_UpdateDefeat(), range, 0x20,
                                count = 4, duration = 20, scale = 2, 1);
            SparkParticle_Spawn(position, RandomFrom(table), range, 0x40, count, duration, scale,
                                1);
            SparkParticle_Spawn(position, RandomFrom(table), range, 0x5E, count, duration, scale,
                                1);
            SparkParticle_Spawn(position, RandomFrom(table), range, 0x80, count, duration, scale,
                                1);
            SparkParticle_Spawn(position, RandomFrom(table), range, 0xC4, count, duration, scale,
                                1);
            SparkParticle_Spawn(position, RandomFrom(table), range, 0xF0, count, duration, scale,
                                1);
        }
        category = actor->category;
        if (((((((category == 4) || (category == 1)) || (category == 5)) || (category == 7)) ||
              (category == 6)) ||
             (category == 0x18)) ||
            (category == 0x19))
            Sound_PlaySong(0xE5);
    }
    {
        register u32 offset asm("r4") = 0x1F8;
        asm("" : "+r"(offset));
        script = *((u16 *)(((u8 *)actor) + offset));
    }
    if (script) {
        values[0] = actor->position.x;
        values[1] = actor->position.y;
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
    if ((!Flag(actor, 0x4000, 0x2D4)) && (((u32)(actor->category - 11)) > 2))
        GameState_SetEventFlag(
            BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    actor->fieldAA = 0;
    {
        u32 mask = 1;
        if (!actor->drawableKind)
            *((u32 *)actor->drawable) |= mask;
        else {
            struct VornDrawableTail {
                u32 unknown0[2];
                u32 flags;
            } *tail;
            tail = (void *)(((u8 *)actor->drawable) + 0x20);
            tail->flags |= mask;
        }
    }
    {
        void *control = SceneActorControl_OnIdle;
        u32 kind = 16;
        actor->field2B2 = 1;
        actor->field2B0 = 0;
        actor->field2B5 = kind;
        actor->target = control;
        actor->field2C8 = 0;
        {
            u32 *flags = &actor->flags114;
            s32 mask = ~1;
            *flags &= mask;
        }
        actor->word104 = 0;
    }
    {
        u32 behavior = 2;
        actor->state2A8 = 1;
        actor->phase2A9 = 0;
        actor->behavior2AA = behavior;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
    ActorShadow_Disable(((u8 *)actor) + 0x320);
}

#else
INCLUDE_ASM("asm/nonmatching", Vorn_UpdateDefeat);
#endif
