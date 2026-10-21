// MATCHING_CONCESSION: register-pinning
#include "golem_2.h"
#include "script_utility.h"

extern const u8 gUnk_08606134[];

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(u8 *);
void Golem_ResetIdleControl(u8 *);
void Camera_RequestShake(u32);
void Skeleton_TryTriggerAction(void *);
void Skeleton_ClearPendingFlag(void *);
void sub_080C53A8(void *);

inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

static inline u32 Flags16(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D4;
    return *(u16 *)((u8 *)a + off) & m;
}

static inline u32 Flags32(struct SceneActorRegistryResetView *a, u32 m)
{
    u32 off = 0x2D0;
    return *(u32 *)((u8 *)a + off) & m;
}

static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a) {
        if (Flags16(a, 0x40) || Flags32(a, 0x20000) || a->health <= 0)
            return 1;
    }
    return 0;
}

void SceneActorEffects_AdvanceSteps(void *, u32);
void SceneActorTerrain_ClearCollisionTimers(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
void ActorShadow_Disable(void *);
void ActorParticles_SpawnWideBurst(void *);
void SceneActorControl_OnIdle(void);
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
void Golem_OnUnusedState(void)
{
}

static inline u32 Flags32At(struct SceneActorRegistryResetView *a, u32 mask)
{
    return *(u32 *)((u8 *)a + 0x2CC) & mask;
}
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

/* Releases the defeated golem, runs its script, and emits the defeat burst. */
void Golem_UpdateDefeat(struct SceneActorRegistryResetView *actor)
{
    s32 values[8];
    struct ContactPackedVector rangeB;
    struct ScriptUtilityArguments arguments;
    u8 *group = (u8 *)&actor->group;
    u16 script;
    if (TakeFlag(&actor->field2B2)) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 10);
        actor->field2B3 = 0;
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(actor, 3, 127, 0);
        SceneActorEffects_AdvanceSteps(actor, 4);
        {
            struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
            u32 *field = &actor->flags2CC;
            u32 mask = ~0x141C0;
            *field &= mask;
            control->counter = 0;
            *(u16 *)((u8 *)control + 0x14) = 0;
            control->timer = 0;
        }
        Sound_PlaySong(0x13E);
        {
            u16 *flags = (u16 *)((u8 *)actor + 0x13A);
            u32 value = *flags;
            u32 mask = 4;
            asm("" : "+r"(value), "+r"(mask));
            *flags = mask | value;
        }
    }
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    if (!actor->field2B3)
        return;
    actor->field2B0 = 1;
    SceneActorBehavior_CallTerrainScript(actor);
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        values[0] = actor->position.x;
        values[1] = actor->position.y;
        values[2] = actor->position.z;
        values[3] = *(s32 *)((u8 *)actor + 0x1FC);
        values[4] = *(s32 *)((u8 *)actor + 0x200);
        values[5] = *(s32 *)((u8 *)actor + 0x204);
        values[6] = *(s32 *)((u8 *)actor + 0x2E0);
        values[7] = actor->id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    if (!Flags16(actor, 0x4000) && (u32)(actor->category - 11) > 2)
        GameState_SetEventFlag(
            BestiaryDisplay_GetEntryForActor(actor->category, actor->paletteOffset));
    if (!Flags32At(actor, 0x400)) {
        u8 category;
        rangeB.x = 80;
        rangeB.y = 60;
        rangeB.z = 60;
        {
            register const u16 *table asm("sl") = gUnk_0203B400;
            s32 duration;
            s32 scale;
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x20, 4,
                                duration = 20, scale = 2, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x40, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x5E, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0x80, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0xC4, 4, duration,
                                scale, 1);
            SparkParticle_Spawn(&actor->position, RandomFrom(table), &rangeB, 0xF0, 4, duration,
                                scale, 1);
        }
        category = actor->category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    {
        void *callback = (void *)SceneActorControl_OnIdle;
        u32 kind = 16;
        u32 zero;
        u32 one;
        u8 *ready = &actor->field2B2;
        zero = 0;
        one = 1;
        *ready = one;
        actor->field2B0 = zero;
        actor->field2B5 = kind;
        actor->target = callback;
        actor->field2C8 = zero;
        {
            u32 *field = &actor->flags114;
            s32 clear = ~1;
            *field &= clear;
        }
        actor->word104 = zero;
        {
            u32 behavior = 2;
            actor->state2A8 = one;
            actor->phase2A9 = zero;
            actor->behavior2AA = behavior;
            actor->counter2AC = zero;
            actor->field2B1 = one;
        }
    }
    ActorShadow_Disable((u8 *)actor + 0x320);
    if (!*((u8 *)actor + 8))
        **(u32 **)((u8 *)actor + 4) |= 1;
    else {
        struct DefeatOwnerGroup {
            u32 unknown[2];
            u32 flags;
        };
        struct DefeatOwner {
            u8 unknown[0x20];
            struct DefeatOwnerGroup group;
        };
        struct DefeatOwner *owner = *(void **)((u8 *)actor + 4);
        struct DefeatOwnerGroup *ownerGroup = &owner->group;
        ownerGroup->flags |= 1;
    }
    ActorParticles_SpawnWideBurst(actor);
    SceneActorPlacement_PlayCategoryEventSound(actor);
}

INCLUDE_ASM("asm/nonmatching", sub_080C9C04);

void Golem_UpdateScriptedAction(struct SceneActorRegistryResetView *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        void (*fn)(void) = SceneActorReaction_PlayAnimationBUntilComplete;
        u8 code = 10;
        u32 zero;
        u32 *f;
        s32 mask;
        u16 *h;
        u16 value;
        s32 off;
        {
            u8 *taken = &obj->field2B2;
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
            obj->target = (void *)fn;
            field += 0x13;
            *((u16 *)(((u8 *)obj) + field)) = zero;
        } while (0);
        f = (u32 *)(((u8 *)obj) + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(((u8 *)obj) + 0x104)) = zero;
        {
            u32 byte = obj->fieldE9;
            obj->direction = byte;
        }
        SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->category);
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
    }
    if (obj->field2B0) {
        Golem_ResetIdleControl((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}

INCLUDE_ASM("asm/nonmatching", sub_080CBF24);

void Golem_InitActionHandlers(struct SceneActorRegistryResetView *actor)
{
    actor->alternateBehaviorCallbacks = (void **)gUnk_08606134;
}

s32 Golem_TickFacingQuadrant(struct SceneActorRegistryResetView *a)
{
    u8 *base = (u8 *)&a->id;
    struct GolemState *s = a->context;
    if (!Unavailable(a)) {
        if (s->countdown626)
            s->countdown626--;
    }
    {
        u32 quadrant = (base[9] + 0x60) >> 6;
        u32 mask = 3;
        mask &= quadrant;
        a->unknown68C = mask;
    }
    a->savedDirection690 = a->direction68E;
    return 1;
}

s32 Golem_UpdateEffectsAndContacts(struct SceneActorRegistryResetView *actor)
{
    struct GolemState *state = actor->context;
    u32 m = 0x40;
    if (!(actor->flags2D4 & m)) {
        u16 *p = &state->shakeTimer6AE;
        if (*p != 0) {
            Camera_RequestShake(state->shakeStrength6AC);
            *p = *p - 1;
        }
    }
    Skeleton_TryTriggerAction(actor);
    Skeleton_ClearPendingFlag(actor);
    sub_080C53A8(actor);
    return 1;
}
