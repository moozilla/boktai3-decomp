// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: defeat control emits effects, runs scripts, and returns the actor to idle.

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: flag-address materialization, late argument registers and zero lifetimes.
#include "scene_actor_reaction.h"
#include "script_utility.h"
void SceneActorMovement_UpdateAirborne(void *);
void SceneActorMovement_UpdateHeight(void *);
void SceneEffect_EmitPositionPattern(void *);
void SceneStimulus_AddUntagged(s32, s32, s32, s32, s32);

/* Mummy defeat: hides the drawable, emits defeat effects, runs the script and returns to idle. */
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
extern u32 gUnk_03005308;
struct BatDrawable {
    u32 flags;
    u8 unknown4[4];
    u8 alpha8, alpha9;
    u8 unknownA[0x3E];
    u8 anim[8];
    u16 mode;
    u8 unknown52[4];
    u16 frame;
};
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline u32 Mummy_HalfFlags(u16 *field, u32 mask)
{
    return *field & mask;
}
static inline void ShowTile(struct SceneActorAnimationTile *tile, u32 mask)
{
    tile->flags |= mask;
}
static inline void HideTile(struct SceneActorAnimationTile *tile)
{
    u32 mask = ~1;
    tile->flags = mask & tile->flags;
}
static inline u32 Random4(void)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    u32 v;
    gUnk_03005308 = index;
    index *= 2;
    v = *(const u16 *)((u32)gUnk_0203B400 + index);
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
    v = *(const u16 *)((u32)table + index);
    v &= 3;
    return v + 1;
}
static inline void StoreSpeed(struct SceneActorRegistryResetView *a, u32 v)
{
    *(u16 *)((u8 *)a + 0x1EA) = v;
}
void Mummy_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 input)
{
    register s32 frame asm("r8") = input;
    s32 values[8];
    struct ContactPackedVector rangeA;
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    struct BatDrawable *drawable = actor->drawable;
    u8 *anim = drawable->anim;
    u16 script;
    u16 *motionFlags;
    register struct ContactCollider *collider asm("r5");
    if (SceneActorReaction_TakePrepared(actor)) {
        u8 *group = (u8 *)(&actor->group);
        if (!Flag(actor, 0x20000, 0x2D0)) {
            if (!(*(((u8 *)actor) + 8))) {
                u32 *flags = actor->drawable;
                s32 mask = ~1;
                *flags = mask & (*flags);
            } else
                HideTile((void *)(((u8 *)actor->drawable) + 0x20));
        }
        {
            struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *flags &= mask;
            control->counter = 0;
            *((u16 *)(((u8 *)control) + 0x14)) = 0;
            control->timer = 0;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        StoreSpeed(actor, 0x20);
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        collider = &actor->colliderA;
        asm("" : "+r"(collider));
        *((u16 *)(((u8 *)actor) + 0x178)) = 0;
        SceneActorPlacement_PlayCategoryEventSound(actor);
        if (Flag(actor, 8, 0x2D4))
            Sound_PlaySong(0x12A);
        {
            collider->flags |= 4;
        }
    }
    if ((Mod(frame, 12) == 0) && Flag(actor, 8, 0x2D4)) {
        rangeA.x = 60;
        rangeA.y = 60;
        rangeA.z = 40;
        SparkParticle_Spawn(&actor->position, Random4(), &rangeA, actor->direction + 128, 10, 8, 2,
                            2);
    }
    if (frame <= 0x22)
        *((u16 *)(anim + 0xE)) = 0;
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    {
        u32 test = Flag(actor, 0x400, 0x2CC);
        motionFlags = &actor->flags2D4;
        if (!test) {
            u8 category;
            if (!Mummy_HalfFlags(motionFlags, 0xA007)) {
                SceneEffect_EmitPositionPattern(actor);
                SceneStimulus_AddUntagged(actor->position.x, actor->position.z, 0x400, 0, 1);
            } else {
                rangeB.x = 80;
                rangeB.y = 60;
                rangeB.z = 60;
                {
                    register const u16 *table asm("sl") = gUnk_0203B400;
                    s32 duration;
                    s32 scale;
                    SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x20, 4,
                                        duration = 20, scale = 2, 1);
                    SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x40, 4,
                                        duration, scale, 1);
                    SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x5E, 4,
                                        duration, scale, 1);
                    SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x80, 4,
                                        duration, scale, 1);
                    SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0xC4, 4,
                                        duration, scale, 1);
                    SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0xF0, 4,
                                        duration, scale, 1);
                }
                category = actor->category;
                if (((((((category == 4) || (category == 1)) || (category == 5)) ||
                       (category == 7)) ||
                      (category == 6)) ||
                     (category == 0x18)) ||
                    (category == 0x19))
                    Sound_PlaySong(0xE5);
            }
        }
    }
    SceneActorBehavior_CallTerrainScript(actor);
    script = *((u16 *)(((u8 *)actor) + 0x1F8));
    if (script) {
        values[0] = actor->position.x;
        values[1] = actor->position.y;
        values[2] = actor->position.z;
        values[3] = *((s32 *)(((u8 *)actor) + 0x1FC));
        values[4] = *((s32 *)(((u8 *)actor) + 0x200));
        {
            register u32 offset asm("r5") = 0x204;
            asm("" : "+r"(offset));
            values[5] = *((s32 *)(((u8 *)actor) + offset));
        }
        values[6] = *((s32 *)(((u8 *)actor) + 0x2E0));
        values[7] = actor->id;
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
        u32 test = (*motionFlags) & mask;
        flags = &actor->flags2D4;
        if ((!test) && (((u32)(actor->category - 11)) > 2))
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        actor->fieldAA = 0;
        zero = 0;
        one = 1;
        {
            u32 mask2 = 0x4000;
            u32 value = *flags;
            masked = value & mask2;
        }
        {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            actor->field2B2 = 1;
            actor->field2B0 = zero;
            actor->field2B5 = kind;
            actor->target = callback;
            actor->field2C8 = 0;
            {
                u32 *field = &actor->flags114;
                s32 clear = ~1;
                *field &= clear;
            }
            actor->word104 = 0;
            {
                u32 behavior = 2;
                actor->state2A8 = 1;
                actor->phase2A9 = zero;
                actor->behavior2AA = behavior;
                actor->counter2AC = 0;
                actor->field2B1 = 1;
            }
        }
    }
    if (!(*(((u8 *)actor) + 8)))
        *((u32 *)actor->drawable) |= 1;
    else
        ShowTile((void *)(((u8 *)actor->drawable) + 0x20), 1);
    ActorShadow_Disable(((u8 *)actor) + 0x320);
}

#else
INCLUDE_ASM("asm/nonmatching", Mummy_UpdateDefeat);
#endif
