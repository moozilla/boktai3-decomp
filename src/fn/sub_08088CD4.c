#include "scene_actor_behavior.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "scene_actor_part_burst.h"

/* Bat defeat state (tentative; compare SceneActorReaction_BeginHiddenDefeat): on entry it
 * releases attachments, shows the drawable and clears the route; every 12 frames it emits
 * sparks while flag 8 is set, then a six-spark burst, the defeat script, the event flag and
 * the switch to SceneActorControl_OnIdle. */
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);

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
static inline u32 Flag(struct ChaseActor *actor, u32 maskInput, u32 off)
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
union HitSparkTable {
    const u16 *table;
    struct {
        u16 offset;
        u8 page;
        u8 region;
    } encoded;
};
static inline const u16 *ReadSparkTable(const union HitSparkTable *r)
{
    return r->table;
}

void Bat_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    s32 values[8];
    struct ContactPackedVector rangeA;
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    struct BatDrawable *drawable = actor->base.drawable;
    u8 *anim = drawable->anim;
    u16 script;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->base.restart = 0;
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
            u32 *field = &actor->base.behaviorFlags;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            control->field14 = 0;
            control->timer = 0;
        }
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            collider->flags |= 4;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (Mod(frame, 12) == 0 && Flag(actor, 8, 0x2D4)) {
        rangeA.x = 60;
        rangeA.y = 60;
        rangeA.z = 40;
        SparkParticle_Spawn(&actor->base.body.position, Random4(), &rangeA,
                            actor->base.body.direction + 128, 10, 8, 2, 2);
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
    if (!actor->base.restart)
        return;
    actor->base.stepDone = 1;
    if (!Flag(actor, 0x400, 0x2CC)) {
        u8 category;
        SceneActorBehavior_CallTerrainScript(actor);
        rangeB.x = 80;
        rangeB.y = 60;
        rangeB.z = 60;
        {
            union HitSparkTable source = {0};
            const u16 *table;
            u8 firstCount;
            source.encoded.offset = (u16)RANDOM_TABLE_ADDR;
            source.encoded.page = (u8)(RANDOM_TABLE_ADDR >> 16);
            source.encoded.region = (u8)(RANDOM_TABLE_ADDR >> 24);
            table = ReadSparkTable(&source);
            firstCount = SceneActorEffects_NextSparkCount(table);
            SceneActorEffects_SpawnDefeatSparks(actor, (const struct ContactPosition *)&rangeB,
                                                table, firstCount);
        }
        category = actor->base.category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    script = (u16)actor->base.script1F8;
    if (script) {
        values[0] = actor->base.body.position.x;
        values[1] = actor->base.body.position.y;
        values[2] = actor->base.body.position.z;
        values[3] = actor->base.parameters1FC[0];
        values[4] = actor->base.parameters1FC[1];
        values[5] = actor->base.parameters1FC[2];
        values[6] = (s32)actor->base.hitOwners;
        values[7] = actor->base.body.id;
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
        u32 test = actor->base.motionFlags & mask;
        flags = &actor->base.motionFlags;
        if (!test && (u32)(actor->base.category - 11) > 2)
            Bestiary_UnlockEntry(
                Bestiary_GetEntryForActor(actor->base.category, actor->base.paletteOffset));
        actor->base.attachmentTimer = 0;
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
            actor->base.field2B2 = one;
            actor->base.stepDone = zero;
            actor->base.controlKind = kind;
            actor->base.target = callback;
            actor->base.field2C8 = masked;
            {
                u32 *field = &actor->base.pendingControlFlags;
                s32 clear = ~1;
                *field &= clear;
            }
            actor->base.pendingControl = masked;
            {
                u32 behavior = 2;
                actor->base.state2A8 = one;
                actor->base.phase2A9 = zero;
                actor->base.behavior2AA = behavior;
                actor->base.counter2AC = masked;
                actor->base.field2B1 = one;
            }
        }
    }
    ActorShadow_Disable((u8 *)actor + 0x320);
}
