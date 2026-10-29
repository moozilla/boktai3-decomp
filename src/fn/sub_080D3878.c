// MATCHING_CONCESSION: register-pinning
#include "global.h"

// confidence=low: defeat control emits effects, runs scripts, and returns the actor to idle.

#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
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
struct MummyDefeatControl {
    u8 unknown0[8];
    u16 timer;
    u8 unknownA[10];
    u16 flags14;
    u8 unknown16[2];
    u32 counter;
};
struct MummyDefeatGroup {
    u8 unknown0[0x4C];
    struct MummyDefeatControl control;
};
struct MummyDefeatActor {
    void *object, *drawable;
    u8 drawableKind, unknown9[0x47];
    u32 id;
    u8 unknown54[5], direction, unknown5A[2];
    struct ContactPosition position;
    u8 unknown64[0x34];
    u8 category, unknown99[0x11];
    u16 fieldAA;
    u8 unknownAC[0x58];
    u32 word104;
    u8 unknown108[12];
    u32 flags114;
    u8 unknown118[0x1C];
    struct ContactCollider colliderA, colliderB;
    u8 unknown1DC[10];
    u16 paletteOffset, field1E8, field1EA;
    u8 unknown1EC[12];
    u16 defeatScript, unknown1FA;
    s32 scriptValues[3];
    u8 unknown208[0x14];
    void *target;
    u8 unknown220[0x88];
    u8 state2A8, phase2A9, behavior2AA, unknown2AB;
    u32 counter2AC;
    u8 field2B0, field2B1, field2B2, field2B3;
    u8 enabled, field2B5, unknown2B6[0x12];
    u16 field2C8, unknown2CA;
    u32 flags2CC, flags2D0;
    u16 flags2D4, flags2D6;
    u32 field2D8, field2DC, field2E0;
    u8 unknown2E4[0x3C];
    u8 shadow[0xB4];
    struct MummyDefeatGroup group;
};
struct MummyDefeatAnimation {
    u8 unknown0[0xE];
    u16 frame;
};
struct MummyDefeatDrawable {
    u32 flags;
    u8 unknown4[0x44];
    struct MummyDefeatAnimation anim;
};
static inline u32 Mummy_MotionFlags(struct MummyDefeatActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2D4 & mask;
}
static inline u32 Mummy_ActorFlags(struct MummyDefeatActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2CC & mask;
}
static inline u32 Mummy_DisplayFlags(struct MummyDefeatActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2D0 & mask;
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
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)((const u16 *)RANDOM_TABLE_ADDR) + index);
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
static inline void StoreSpeed(struct MummyDefeatActor *a, u32 v)
{
    a->field1EA = v;
}
void Mummy_UpdateDefeat(struct MummyDefeatActor *actor, s32 frame)
{
    s32 values[8];
    struct ContactPackedVector rangeA;
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    struct MummyDefeatDrawable *drawable = actor->drawable;
    struct MummyDefeatAnimation *anim = &drawable->anim;
    u16 script;
    u16 *motionFlags;
    struct ContactCollider *collider;
    if (SceneActorReaction_TakePrepared((void *)actor)) {
        struct MummyDefeatGroup *group = &actor->group;
        if (!Mummy_DisplayFlags(actor, 0x20000)) {
            if (!actor->drawableKind) {
                u32 *flags = actor->drawable;
                s32 mask = ~1;
                *flags = mask & (*flags);
            } else
                HideTile(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile);
        }
        {
            struct MummyDefeatControl *control = &group->control;
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *flags &= mask;
            control->counter = 0;
            control->flags14 = 0;
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
        actor->colliderA.responseValue = 0;
        SceneActorPlacement_PlayCategoryEventSound(actor);
        if (Mummy_MotionFlags(actor, 8))
            Sound_PlaySong(0x12A);
        {
            collider->flags |= 4;
        }
    }
    if (Mod(frame, 12) == 0 && Mummy_MotionFlags(actor, 8)) {
        rangeA.x = 60;
        rangeA.y = 60;
        rangeA.z = 40;
        SparkParticle_Spawn(&actor->position, Random4(), &rangeA, actor->direction + 128, 10, 8, 2,
                            2);
    }
    if (frame <= 0x22)
        anim->frame = 0;
    SceneActorMovement_UpdateAirborne(actor);
    SceneActorMovement_UpdateHeight(actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    {
        u32 test = Mummy_ActorFlags(actor, 0x400);
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
                    register const u16 *table asm("sl") = gRandomTable;
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
                if (category == 4 || category == 1 || category == 5 || category == 7 ||
                    category == 6 || category == 0x18 || category == 0x19)
                    Sound_PlaySong(0xE5);
            }
        }
    }
    SceneActorBehavior_CallTerrainScript(actor);
    script = actor->defeatScript;
    if (script) {
        values[0] = actor->position.x;
        values[1] = actor->position.y;
        values[2] = actor->position.z;
        values[3] = actor->scriptValues[0];
        values[4] = actor->scriptValues[1];
        values[5] = actor->scriptValues[2];
        values[6] = actor->field2E0;
        values[7] = actor->id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    {
        u32 mask = 0x4000;
        u32 zero;
        u32 test = (*motionFlags) & mask;
        if (!test && (u32)(actor->category - 11) > 2)
            GameState_SetEventFlag(
                BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
        actor->fieldAA = 0;
        zero = 0;
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
    if (!actor->drawableKind)
        *((u32 *)actor->drawable) |= 1;
    else
        ShowTile(&((struct SceneActorAnimationTileDrawable *)actor->drawable)->tile, 1);
    ActorShadow_Disable(actor->shadow);
}
