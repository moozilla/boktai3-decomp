#include "global.h"
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "scene_actor_part_burst.h"
#include "scene_actor_chase.h"
#include "scene_actor_animation.h"
#include "scene_actor_stimulus.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_delayed_response.h"
#include "scene_actor_control_phase.h"
#include "debug.h"
#include "mummy.h"
#include "scene_actor_turn.h"

// confidence=low: defeat control emits effects, runs scripts, and returns the actor to idle.

void SceneActorMovement_UpdateAirborne(void *);

void SceneActorMovement_UpdateHeight(void *);

void SceneEffect_EmitPositionPattern(struct ChaseActor *emitter);

void SceneStimulus_AddUntagged(s32, s32, s32, s32, s32);

/* Mummy defeat: hides the drawable, emits defeat effects, runs the script and returns to idle. */

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

void SceneActorEffects_AdvanceSteps(void *, u32);

void *Bestiary_GetEntryForActor(u32, u32);

void Bestiary_UnlockEntry(void *);

void SceneActorBehavior_CallTerrainScript(void *);

void SceneActorPlacement_PlayCategoryEventSound(void *);

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
                    union HitSparkTable source = {0};
                    const u16 *table;
                    source.encoded.offset = (u16)RANDOM_TABLE_ADDR;
                    source.encoded.page = (u8)(RANDOM_TABLE_ADDR >> 16);
                    source.encoded.region = (u8)(RANDOM_TABLE_ADDR >> 24);
                    table = ReadSparkTable(&source);
                    {
                        u8 firstCount = SceneActorEffects_NextSparkCount(table);
                        SceneActorEffects_SpawnDefeatSparks((void *)actor,
                                                            (const struct ContactPosition *)&rangeB,
                                                            table, firstCount);
                    }
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
            Bestiary_UnlockEntry(Bestiary_GetEntryForActor(actor->category, actor->paletteOffset));
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

// confidence=low: control reacquires a player route once the context delay clears.

static inline void SetHalf(u16 *field, u32 bits)
{
    s16 mask = bits;
    mask |= *field;
    *field = mask;
}

static inline u32 ClearWordValue(u32 *field, u32 bits)
{
    return *field & ~bits;
}

static inline u16 MotionFlags(struct ChaseActor *actor, u32 mask)
{
    u16 *field = &actor->base.motionFlags;
    u32 value = *field;
    return value & mask;
}

static inline u8 Test16(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}

struct MummyRandomTable {
    const u16 *samples;
};

static inline u32 PickTurn(u32 dir, const u16 *randomSamples)
{
    s32 i;
    struct MummyRandomTable table = {randomSamples};
    for (i = 0; i < 4; i++) {
        const u16 *samples = (&table)->samples;
        u32 index = (gRandomIndex + 1) & 1023;
        u32 d;
        gRandomIndex = index;
        d = samples[index];
        d &= 3;
        d <<= 6;
        if (d != dir)
            return d;
    }
    return dir;
}

struct MummyTurnBytes {
    u8 direction : 8;
    u8 saved : 8;
    u8 remaining : 8;
};

struct MummySavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};

static inline void Turn(u8 saved, void *ctx, const u16 *randomSamples)
{
    struct ChaseActor *actor = ctx;
    u8 *group = (u8 *)&actor->group.common;
    {
        u32 mask = 0x400;
        if (((struct ChaseActor *)ctx)->base.motionFlags & mask)
            return;
    }
    if (Test16((u16 *)((u8 *)ctx + 0x414), 4))
        return;
    ((struct MummyTurnBytes *)(group + 0x204))->direction =
        PickTurn(actor->base.body.direction, randomSamples);
    {
        struct MummySavedControl *save = (struct MummySavedControl *)(group + 0x8C);
        save->target = actor->base.target;
        save->kind = actor->base.controlKind;
        save->state = actor->base.state2A8;
        save->phase = actor->base.phase2A9;
        save->behavior = actor->base.behavior2AA;
    }
    group[0x205] = saved;
    Chase_SetCtl(actor, *(void **)(group + 0x21C), 0x1B);
    Chase_SetState(actor, 3, 1, 0);
    group[0x206] = 6;
}

static inline s32 PlayerSlot(u8 *target)
{
    if (target[8] <= 3)
        return target[8];
    return -1;
}

static inline u8 Arrive(void *obj)
{
    struct ContactPosition target;
    struct ChaseActor *actor = obj;
    u8 *group = (u8 *)&actor->group.common;
    struct ChaseTarget **slot = (struct ChaseTarget **)((u8 *)actor + 0xDC);
    s32 cell;
    s32 player;
    const u16 *randomSamples = gRandomTable;
    Chase_SnapToRoute(*(struct ContactPosition **)((u8 *)actor + 0x5D4), (u8 *)actor + 0x448);
    if (Chase_Test(gEngineFlags, 0x4800)) {
        struct ChaseTarget *t = *slot;
        if (!t)
            goto invoke;
        player = PlayerSlot((u8 *)t);
        if (player < 0) {
            goto invoke;
        }
        cell = ((u16 *)gPlayers[player]->followTarget)[3];
    } else {
        struct ChaseTarget *t = *slot;
        cell = Chase_CellIndex(t->position.x >> 8, t->position.z >> 8);
        player = 0;
    }
    if (cell != actor->base.terrainCell && !Chase_Bits32(&gPlayers[player]->statusFlags, 0x20000))
        goto reroute;
    if (MotionFlags(actor, 0x400))
        return 0;
    {
        struct ChaseFlags40 *flags = (void *)(group + 0x40);
        s32 mask = ~4;
        flags->bits = mask & flags->bits;
    }
    if (MotionFlags(actor, 0x80)) {
        Chase_SetCtl(actor, *(void **)(group + 0x48), 0x1A);
        Chase_SetBehavior(actor, 12);
    } else {
        Chase_SetCtl(actor, actor->base.onAlternateEntry, 1);
        Chase_SetBehavior(actor, 5);
        Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
        (*(struct ContactPosition **)(group + 0x200))->y =
            Chase_HeightAt(Chase_CellAt(*(struct ContactPosition **)(group + 0x200)),
                           *(struct ContactPosition **)(group + 0x200));
        (*(struct ContactPosition **)(group + 0x200))->pad = 0;
    }
    {
        u8 saved = actor->base.body.direction;
        Turn(saved, obj, randomSamples);
    }
    SceneActorTerrain_RequestRetreatRoute(obj);
    actor->base.field2C2 = 0x20;
    return 0;
reroute: {
    struct ContactPosition *targetPointer = &target;
    s32 x = Mod(cell, *(s16 *)(gTerrainMap->cells + 4));
    s32 z = Div(cell, *(s16 *)(gTerrainMap->cells + 4));
    u8 *data;
    targetPointer->x = (x << 8) + 0x80;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else
        data = gTerrainMap->cells + (cell * 4 + 12);
    targetPointer->y = (*data & 15) << 8;
    targetPointer->z = (z << 8) + 0x80;
    if (Chase_TryRoute(obj, *(void **)(group + 0x1FC), &actor->base.body.position, &target))
        goto moved;
    goto invoke;
}
invoke:
    Callback_InvokeActor(actor, *(void **)(group + 0x224));
    return 0;
moved:
    return 1;
}

// confidence=low: control callback reacquires the player and resumes a route.

u8 BouncingObject_RefreshNearbyHeading(struct ChaseActor *);

static inline u8 TakeControlEntry(struct ChaseActor *actor)
{
    if (actor->base.field2B2) {
        actor->base.field2B2 = 0;
        return 1;
    }
    return 0;
}

void Mummy_UpdatePlayerRoute(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    u8 *group = (u8 *)(&actor->group.common);
    if (TakeControlEntry(actor)) {
        u32 mask;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        mask = 4;
        actor->base.statusFlags |= mask;
    }
    if (BouncingObject_RefreshNearbyHeading(actor))
        Chase_SetBehavior(actor, 16);
    if (actor->base.behavior2AA == 16)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 1);
    else if (actor->base.behavior2AA == 18)
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    actor->base.field2C2 = 32;
    if (!(*((u16 *)(body + 0x6d6)))) {
        actor->base.tracking.target = ((u32)gPlayers[0]) + 0x24;
        {
            u32 mask = 4;
            SetHalf((u16 *)(group + 0x40), mask);
        }
        actor->base.statusFlags = ClearWordValue(&actor->base.statusFlags, 4);
        {
            struct ChaseFlags40 *field = (void *)(body + 0x6f2);
            s32 mask = ~0x1400;
            field->bits = mask & field->bits;
        }
        if (Arrive(actor)) {
            Chase_SetCtl(actor, *((void **)(group + 0x48)), 0x25);
            Chase_SetBehavior(actor, 12);
        }
    } else
        actor->base.counter2AC++;
}

// confidence=low: consumes a nearby stimulus and attempts a tagged-position route.

u8 SceneStimulus_FindOverlap(s16, s16, u16, u8, u16);

void SceneActorPosition_ResolveTaggedPosition(void *, void *, void *, s8);

u8 SceneActor_TryTrackNearbyPlayer(struct ChaseActor *, struct ContactPosition *, u32);

void SceneGrid_SnapPositionToCellCenter(struct ContactPosition *, u32);

void SceneActorAttachment_RetargetNearestPlayer(void *, void *);

struct MummyStimulusInfo {
    u16 field4C, radius;
    u32 field50;
    u16 timer : 16;
    u8 phase[1], kind;
};

struct MummyStimulusRouteLink {
    u8 unknown0[8];
    u16 *point;
};

struct MummyStimulusGroup {
    u8 unknown0[0x40];
    u16 flags, timerLimit, movementTimer;
    u8 unknown46[6];
    struct MummyStimulusInfo stimulus;
    struct ContactPosition target;
    u8 unknown60[4];
    u32 cell;
    void *control;
    u8 unknown6C[8];
    struct MummyStimulusRouteLink link;
    u8 unknown80[0x17C];
    void *route;
    struct ContactPosition *destination;
    u8 unknown204[0x20];
    void *fallback;
};

struct MummyStimulusContext {
    u8 unknown0[0x6EA];
    u8 phase[1];
    u8 unknown6EB[7];
    u16 flags;
};

static inline u8 Mummy_TryRoute(u16 *flags, u32 mask, void *route, struct ContactPosition *from,
                                struct ContactPosition *to)
{
    s32 result = Nav_IsPositionInMesh(route, from, to);
    if (!result) {
        *flags &= ~mask;
        return 0;
    }
    result = Terrain_ApplyBlocked(route, from, to);
    if (result) {
        if (result != 2)
            Memory_Clear(from + 1, 8);
        return 1;
    }
    *flags &= ~mask;
    return 0;
}

static inline u8 Mummy_TestContactFlags(struct SceneActorReactionMotion *motion, u32 mask)
{
    if (motion->flags & mask)
        return 1;
    return 0;
}

static inline u32 ActorWordBits(struct ChaseActor *a, u32 mask)
{
    return a->base.behaviorFlags & mask;
}

static inline u16 ActorHalfBits(struct ChaseActor *a, u32 mask)
{
    u16 value = a->base.motionFlags;
    value &= mask;
    return value;
}

static inline u32 ClearActivity(u32 *flags, u32 mask)
{
    u32 value = *flags;
    value &= mask;
    *flags = value;
    return value;
}

static inline u8 Mummy_ShouldSkipStimulus(u8 *object)
{
    struct ChaseActor *a = (void *)object;
    u32 motion;
    motion = ClearActivity(&a->base.statusFlags, 0xFEFFFFFF);
    if (!ActorWordBits(a, 0x200000))
        return 1;
    if (motion & 4)
        return 1;
    if (ActorWordBits(a, 0x100) || ActorHalfBits(a, 0xE13F) ||
        Mummy_TestContactFlags((void *)&a->base.tracking.target, 16))
        return 1;
    if (((struct ChaseActor *)object)->base.hitLockTimer)
        return 1;
    {
        u16 *timer = &((struct ChaseActor *)object)->base.colliderA.responseValue;
        object = (u8 *)timer;
        if (*timer)
            return 1;
    }
    return 0;
}

u32 Mummy_TryStimulusRoute(struct ChaseActor *a)
{
    struct MummyStimulusGroup *group;
    struct MummyStimulusContext *body;
    s8 tag;
    u32 started;
    u16 *motionFlags;
    if (Mummy_ShouldSkipStimulus((u8 *)a))
        goto failed;
    group = (void *)&a->group.common;
    ((struct MummyStimulusGroup *)&a->group.common)->stimulus.phase[0] = 0;
    started = 0;
    body = a->base.context;
    tag = SceneStimulus_FindOverlap(
        a->base.body.position.x, a->base.body.position.z,
        ((struct MummyStimulusGroup *)&a->group.common)->stimulus.radius,
        ((struct MummyStimulusGroup *)&a->group.common)->stimulus.kind, (u16)a->base.body.id);
    if (tag < 0)
        goto failed;
    if (!((struct MummyStimulusGroup *)&a->group.common)->stimulus.timer) {
        ((struct MummyStimulusGroup *)&a->group.common)->stimulus.timer = 180;
        Chase_SetCtl(a, group->control, 9);
        Chase_SetBehavior(a, 11);
        {
            u32 mask = 0x4000;
            body->flags = mask | (body->flags);
        }
        started = 1;
    }
    if (SceneActorStimulus_CaptureMotionFlags(a, 0x80, &motionFlags)) {
        SceneActorStimulus_ExtendGate(a);
    }
    SceneActorPosition_ResolveTaggedPosition(&group->stimulus, &a->base.body.id, group->route, tag);
    if (SceneActor_TryTrackNearbyPlayer(a, &group->target, group->cell))
        goto success;
    Chase_SnapToRoute(group->destination, (u8 *)&group->link);
    (group->destination)->y = a->base.body.position.y;
    (group->destination)->pad = 0;
    SceneGrid_SnapPositionToCellCenter(&group->target, group->cell);
    if (!Mummy_TryRoute(motionFlags, 0x200, group->route, &a->base.body.position, &group->target)) {
        Callback_InvokeActor(a, group->fallback);
    success:
        return 1;
    }
    {
        u32 mask = 16;
        a->base.behaviorFlags |= mask;
    }
    a->base.field2C2 = Div(64, 3);
    {
        u32 mask = 0xFF7FFFFF;
        a->base.behaviorFlags = mask & a->base.behaviorFlags;
    }
    body->phase[0] = 4;
    {
        u32 mask = 0x200;
        *motionFlags = mask | *motionFlags;
    }
    {
        u32 mask = 0x1000000;
        a->base.statusFlags |= mask;
    }
    SceneActorAttachment_RetargetNearestPlayer(a, &group->target);
    if (started)
        goto success;
failed:
    return 0;
}

void Mummy_UpdateAttackCollider(struct ChaseActor *actor);

struct SUpdateMovementTimers;

void Mummy_UpdateMovementTimers(struct SUpdateMovementTimers *s);

struct SUpdateMovementCooldown;

void Mummy_UpdateMovementCooldown(struct SUpdateMovementCooldown *s);

void Mummy_UpdateBurnDamage(void *);

struct SUpdateTemporaryFlagTimer;

void Mummy_UpdateTemporaryFlagTimer(struct SUpdateTemporaryFlagTimer *s);

s32 Mummy_UpdateBehavior(void *p)
{
    Mummy_UpdateAttackCollider(p);
    Mummy_UpdateMovementTimers(p);
    Mummy_UpdateMovementCooldown(p);
    Mummy_UpdateBurnDamage(p);
    Mummy_UpdateTemporaryFlagTimer(p);
    return 1;
}

struct SClearInactiveActionFlags;

void Mummy_ClearInactiveActionFlags(struct SClearInactiveActionFlags *s);

s32 Mummy_UpdateSpecialFlags(void *actor)
{
    Mummy_ClearInactiveActionFlags(actor);
    return 1;
}

void SceneActorAnimation_UpdateDirectionalEntry(u8 *p, s32 arg1);

void SceneActorBehavior_CallActionScript(u8 *);

void Mummy_EnterIdleControl(struct ChaseActor *actor);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}

struct SUpdateScriptedAction {
    u8 pad0[0x59];
    u8 at59;
    u8 pad1[0x98 - 0x5A];
    u8 at98;
    u8 pad2[0xE9 - 0x99];
    u8 atE9;
    u8 pad3[0x104 - 0xEA];
    u32 at104;
    u8 pad4[0x114 - 0x108];
    u32 flags114;
    u8 pad5[0x21C - 0x118];
    void (*callback)(void);
    u8 pad6[0x2AC - 0x220];
    u32 counter;
    u8 done;
    u8 queued;
    u8 taken;
    u8 pad7[2];
    u8 code;
    u8 pad8[0x2C8 - 0x2B6];
    u16 at2C8;
    u8 pad9[0x2D4 - 0x2CA];
    u16 flags2D4;
    u8 pad10[0x414 - 0x2D6];
    u16 flags414;
    u16 at416;
    u16 at418;
};

void Mummy_UpdateScriptedAction(struct SUpdateScriptedAction *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        void (*fn)(void) = SceneActorAnimation_UpdateDirectionalEntry;
        u8 code = 10;
        u32 zero;
        u32 *f;
        s32 mask;
        u16 *h;
        u16 value;
        s32 off;
        {
            u8 *taken = &obj->taken;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 field = 0x2B0;
            do {
                ((u8 *)obj)[field] = zero;
            } while (0);
            field += 5;
            ((u8 *)obj)[off = field] = code;
            obj->callback = fn;
            field += 0x13;
            *((u16 *)(((u8 *)obj) + field)) = zero;
        } while (0);
        f = (u32 *)(((u8 *)obj) + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(((u8 *)obj) + 0x104)) = zero;
        {
            u32 byte = obj->atE9;
            obj->at59 = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->at98);
        SceneActorBehavior_CallActionScript((u8 *)obj);
        h = (u16 *)(((u8 *)obj) + 0x414);
        {
            u32 flag = 4;
            flag |= *h;
            *h = flag;
        }
        {
            u32 flag = 0x80;
            u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
            flag |= *flags;
            *flags = flag;
        }
        off = 0x416;
        value = *((u16 *)(((u8 *)obj) + off));
        off += 2;
        *((u16 *)(((u8 *)obj) - (-off))) = value;
    } else if (obj->done) {
        Mummy_EnterIdleControl((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

void Mummy_UpdateDelayedPursuit(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u32 zero;
    SceneActorControl_TakeStateEntry(actor);
    zero = (u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (zero)
        return;
    if ((s32)actor->base.counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
    SceneActorChase_SetState(actor, 3, 1, zero);
    SceneActorChase_SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    if (SceneActorChase_MotionFlags(actor, 0x80)) {
        SceneActorChase_SetActorFlags(actor, 0x80);
        if (!SceneActorChase_Arrive(actor)) {
            if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                          &actor->base.tracking.target->position)) {
                Callback_InvokeActor(actor, group->fallback);
                return;
            }
        }
        SceneActorChase_SetFlagComponent(&group->common.gate.flags, 4);
        SceneActorDelay_Prepare((void *)&group->saved, group->common.gate.control, 12, 30);
    } else {
        SceneActorChase_SetActorFlags(actor, 0x10);
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        SceneActorChase_SnapToRoute(group->destination, &group->homeRoute);
        if (!SceneActorChase_TryRoute(actor, group->route, &actor->base.body.position,
                                      group->destination)) {
            SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 0);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else
            group->destination->pad = 1;
    }
    return;
waiting:
    actor->base.counter2AC++;
}

extern const u8 gUnk_08606164[];

struct SInitActionHandlers {
    u8 filler[648];
    const u8 *p;
};

void Mummy_InitActionHandlers(struct SInitActionHandlers *s)
{
    s->p = gUnk_08606164;
}

// confidence=low: waits for a timer, then replaces response slot 0 and restarts the behavior.

void Mummy_WaitReplaceResponse(struct ChaseActor *actor)
{
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    if (Chase_TakeArrived(actor)) {
        DEBUG_PRINT();
    }
    if (!(u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque)) {
        if ((s32)actor->base.counter2AC >= group->gate.counter) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->base.category);
            Chase_SetBehavior(actor, 2);
        } else {
            actor->base.counter2AC++;
        }
    }
}

// Tentative: consumes route probes and selects navigation or a descriptor turn.

s32 NavMesh_ConsumeProbeEvent(void *);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, s32, s32, s32);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, s32, s32, s32);

u8 SceneActorRoute_TryRecoveryDetour(void *);

u8 Mummy_RequestStraightRoute(struct ChaseActor *actor);

static inline u32 Mummy_MotionBits(struct ChaseActor *actor, u32 mask)
{
    return actor->base.motionFlags & mask;
}

static inline void Mummy_ApplyDuration(struct ChaseActor *a)
{
    u16 *scale;
    u32 mask = 0xA02F;
    if (a->base.motionFlags & mask) {
        u16 *p = &a->base.field2C2;
        *p = 64;
        scale = p;
    } else {
        u16 *q = &a->base.field2C2;
        u16 value = *q;
        scale = q;
        if (!value)
            *scale = 64;
    }
    if (!a->base.drawableKind)
        Anim_SetDurationScale(
            ((struct SceneActorAnimationSequenceDrawable *)a->base.drawable)->sequence, *scale);
    else
        Anim_SetFrameDurationScale(
            &((struct SceneActorAnimationTileDrawable *)a->base.drawable)->tile, *scale);
}

struct MummyProbeGroup {
    struct SceneActorChaseGroup *group;
};

static inline struct MummyProbeGroup Mummy_ProbeGroup(void *object)
{
    struct ChaseActor *actor = object;
    struct MummyProbeGroup owner;
    owner.group = SceneActorChase_GroupOf(actor);
    return owner;
}

static inline void Mummy_BeginProbeTurn(void *object, u32 saved, const u16 *randomSamples)
{
    struct ChaseActor *actor = object;
    struct MummyProbeGroup owner = Mummy_ProbeGroup(object);
    struct SceneActorChaseGroup *group = owner.group;
    if (!Mummy_MotionBits(actor, 0x400) &&
        !SceneActorChase_TestField((u16 *)&actor->group.common.gate.flags, 4)) {
        group->turn.direction =
            SceneActorChase_RandomDirectionExcept(actor->base.body.direction, randomSamples);
        SceneActorChase_SaveControl(&group->saved, actor);
        group->turn.savedDirection = saved;
        SceneActorChase_SetControl(actor, group->turnControl, 0x1B);
        SceneActorChase_SetState(actor, 3, 1, 0);
        group->turn.turnsRemaining = 2;
    }
}

static inline void Mummy_SetBehavior(struct ChaseActor *actor, u32 behavior)
{
    actor->base.state2A8 = 0;
    actor->base.phase2A9 = 0;
    actor->base.behavior2AA = behavior;
    SceneActorChase_ResetStateEntry(actor);
}

void Mummy_UpdateProbeNavigation(struct ChaseActor *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    struct Mummy *body = actor->base.context;
    s32 speed;
    const u16 *randomSamples = gRandomTable;
    if (SceneActorControl_TakeStateEntry(actor)) {
        u16 *field = &body->flags6F2;
        u32 mask = ~0x1000;
        u32 value = *field;
        mask &= value;
        *field = mask;
    }
    if ((u8)Callback_InvokeActor(actor, actor->base.contactQuery.opaque))
        return;
    speed = Callback_InvokeActor(actor, actor->base.animationQuery.opaque);
    Mummy_ApplyDuration(actor);
    if (NavMesh_ConsumeProbeEvent(group->route)) {
        if (!SceneActorBehavior_RequestDescriptorTurn(actor, group->route, 32, 32, speed))
            goto increment;
        if (!SceneActorChase_TestField((u16 *)&group->common.gate.flags, 4)) {
            if (SceneActorRoute_TryRecoveryDetour(actor))
                return;
            if (SceneActorChase_TestField((u16 *)&group->common.gate.flags, 1)) {
                u32 saved = ((actor->base.nextDirection + 5) & 7) << 5;
                Mummy_BeginProbeTurn(actor, saved, randomSamples);
                return;
            }
        }
        Mummy_SetBehavior(actor, 13);
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, group->route, 32, 32, speed)) {
        if (SceneActorChase_Bits32(&((struct SceneActorChasePlayer *)gUnknown_02000580[0])->flags,
                                   8) &&
            !body->pursuitDelay) {
            if (Mummy_RequestStraightRoute(actor))
                return;
        }
        Mummy_SetBehavior(actor, 14);
        return;
    }
increment:
    actor->base.counter2AC++;
}
