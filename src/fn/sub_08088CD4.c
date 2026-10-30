// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"

/* Bat defeat state (tentative; compare SceneActorReaction_BeginHiddenDefeat): on entry it
 * releases attachments, shows the drawable and clears the route; every 12 frames it emits
 * sparks while flag 8 is set, then a six-spark burst, the defeat script, the event flag and
 * the switch to SceneActorControl_OnIdle. */
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
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
static inline u32 Random4(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)gRandomTable + index);
    v &= 3;
    return v + 1;
}
/* The first spark's index arithmetic; the burst below passes the table explicitly. */
static inline u32 RandomFrom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)table + index);
    v &= 3;
    return v + 1;
}
void Bat_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    s32 values[8];
    struct ContactPackedVector rangeA;
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    struct BatDrawable *drawable = actor->drawable;
    u8 *anim = drawable->anim;
    u16 script;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        drawable->flags |= 2;
        SceneActorPlacement_PlayCategoryEventSound(actor);
        if (Flag(actor, 8, 0x2D4))
            Sound_PlaySong(0x12A);
        {
            struct SceneActorRegistryControl *control = &actor->group.common.control;
            u32 *field = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            *(u16 *)((u8 *)control + 0x14) = 0;
            control->timer = 0;
        }
        {
            struct ContactCollider *collider = &actor->colliderA;
            collider->flags |= 4;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (Mod(frame, 12) == 0 && Flag(actor, 8, 0x2D4)) {
        rangeA.x = 60;
        rangeA.y = 60;
        rangeA.z = 40;
        SparkParticle_Spawn(&actor->body.position, Random4(), &rangeA, actor->body.direction + 128, 10, 8, 2,
                            2);
    }
    if (frame <= 0x22)
        *(u16 *)(anim + 0xE) = 0;
    if (*(u16 *)(anim + 8) <= 2) {
        drawable->alpha8 = 0x46;
        drawable->alpha9 = 0x46;
    } else {
        drawable->alpha8 = 0x40;
        drawable->alpha9 = 0x40;
    }
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    if (!Flag(actor, 0x400, 0x2CC)) {
        u8 category;
        SceneActorBehavior_CallTerrainScript(actor);
        rangeB.x = 80;
        rangeB.y = 60;
        rangeB.z = 60;
        {
            register const u16 *table asm("sl") = gRandomTable;
            s32 duration;
            s32 scale;
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x20, 4,
                                duration = 20, scale = 2, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x40, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x5E, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0x80, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0xC4, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->body.position, RandomFrom(table), &rangeB, 0xF0, 4, duration,
                                scale, 1);
        }
        category = actor->category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        values[0] = actor->body.position.x;
        values[1] = actor->body.position.y;
        values[2] = actor->body.position.z;
        values[3] = *(s32 *)((u8 *)actor + 0x1FC);
        values[4] = *(s32 *)((u8 *)actor + 0x200);
        values[5] = *(s32 *)((u8 *)actor + 0x204);
        values[6] = *(s32 *)((u8 *)actor + 0x2E0);
        values[7] = actor->body.id;
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
        if (!test && (u32)(actor->category - 11) > 2)
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        actor->fieldAA = 0;
        zero = 0;
        drawable->alpha8 = 0x40;
        drawable->alpha9 = 0x40;
        drawable->flags = (drawable->flags & ~2) | 1;
        one = 1;
        {
            u32 mask2 = 0x4000;
            u32 value = *flags;
            masked = value & mask2;
        }
        if (!masked) {
            void *callback = (void *)SceneActorControl_OnIdle;
            u32 kind = 16;
            actor->field2B2 = one;
            actor->field2B0 = zero;
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
    ActorShadow_Disable((u8 *)actor + 0x320);
}
