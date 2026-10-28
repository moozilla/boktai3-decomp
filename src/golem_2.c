// MATCHING_CONCESSION: register-pinning
#include "golem_2.h"
#include "script_utility.h"
#include "engine.h"
#include "player.h"

extern const u8 gUnk_08606134[];

void SceneActorReaction_PlayAnimationBUntilComplete(void);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(u8 *);
void Golem_ResetIdleControl(u8 *);
void Camera_RequestShake(u32);
void Skeleton_TryTriggerAction(void *);
void Skeleton_ClearPendingFlag(void *);
void Golem_UpdateAttackProperties(void *);

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
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
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

#define GA(T, off) (*(T *)((u8 *)a + (off)))
#define GB(T, off) (*(T *)((u8 *)base + (off)))
#define GC(T, off) (*(T *)((u8 *)ctx + (off)))

extern u8 *gUnknown_02000580[];
void SceneActorRoute_CommitReachableDestination(void *);

/* terrain height of a cell id (shared with the Ivy/Axe loaders) */
static inline s32 Golem_CellHeight(u32 cell, u8 *pos)
{
    u8 *p;
    s32 b, h;

    p = TerrainModifiers_FindCellOverride(cell, 1);
    if (p)
        p += 4;
    else
        p = (u8 *)((u32 *)gTerrainMap)[1] + cell * 4 + 0xC;
    b = *p;
    h = (b & 0xF) << 8;
    if ((b >> 4) == 1)
        h -= pos[4];
    else if ((b >> 4) == 2)
        h -= pos[0];
    return h;
}

/* (x, z) in 1/256 units -> cell id; 0 when outside the map */
static inline u32 Golem_CellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return *(u16 *)((u8 *)gTerrainMap + 0x24 + z * 2) + x;
}

static inline s32 Golem_PosHeight(u16 *pos)
{
    s32 x = (s16)pos[0] >> 8;
    s32 z = (s16)pos[2] >> 8;

    return Golem_CellHeight(Golem_CellIndex(x, z), (u8 *)pos);
}

/* the ROM repeats this store sequence everywhere a control state is entered */
static inline void Golem_SetControl(u8 *a, u32 kind, void *target)
{
    GA(u8, 0x2B2) = 1;
    GA(u8, 0x2B0) = 0;
    GA(u8, 0x2B5) = kind;
    GA(void *, 0x21C) = target;
    GA(u16, 0x2C8) = 0;
    GA(u32, 0x114) &= ~1;
    GA(u32, 0x104) = 0;
}

static inline void Golem_SetState(u8 *a, u32 state, u32 phase, u32 behavior)
{
    GA(u8, 0x2A8) = state;
    GA(u8, 0x2A9) = phase;
    GA(u8, 0x2AA) = behavior;
    GA(u32, 0x2AC) = 0;
    GA(u8, 0x2B1) = 1;
}

/* Nav probe + apply; clears the 0x200 flag when the destination is rejected */
static inline u32 Golem_ApplyBlocked(u8 *a, void *mesh, void *q)
{
    s32 r;

    if (Nav_IsPositionInMesh(mesh, a + 0x5C, q) &&
        (r = Terrain_ApplyBlocked(mesh, a + 0x5C, q)) != 0) {
        if (r != 2)
            Memory_Clear(a + 0x64, 8);
        return 1;
    }
    GA(u16, 0x2D4) &= 0xFDFF;
    return 0;
}

/* same probe, second spelling (the ROM keeps the two inlined copies apart) */
static inline u32 Golem_ApplyBlocked2(u8 *a, void *mesh, void *q)
{
    s32 r;
    u32 ok = 0;

    if (Nav_IsPositionInMesh(mesh, a + 0x5C, q)) {
        r = Terrain_ApplyBlocked(mesh, a + 0x5C, q);
        if (r != 0) {
            if (r != 2)
                Memory_Clear(a + 0x64, 8);
            ok = 1;
        }
    }
    if (!ok)
        GA(u16, 0x2D4) &= 0xFDFF;
    return ok;
}

/* Inlined four times: pick where the golem goes after the player-cell check.
   Returns 0 when it already invoked the fallback callback. */
static inline u32 Golem_Reposition(u8 *a, u32 variant)
{
    u8 *base = a + 0x3D4;
    u8 *ctx = GA(u8 *, 0x3D0);
    u16 *pos;
    u16 *src;
    u8 *obj;
    u32 cell;
    u32 res;
    s32 slot;
    s32 i;
    s32 dir;
    u32 v;
    u8 *sav;
    u8 *route;
    u16 q[4];
    u32 w;
    u32 sx, sz;

    pos = GA(u16 *, 0x5D4);
    src = *(u16 **)(a + 0x450);
    pos[0] = src[0];
    pos[2] = src[1];
    pos[1] = Golem_PosHeight(pos);

    if (gEngineFlags & 0x4800) {
        obj = GA(u8 *, 0xDC);
        slot = -1;
        if (obj) {
            if (obj[8] <= 3)
                slot = obj[8];
        }
        if (!obj || slot < 0)
            goto callback;
        cell = *(u16 *)(*(u8 **)(gUnknown_02000580[slot] + 0x40) + 6);
    } else {
        obj = GA(u8 *, 0xDC);
        cell = Golem_CellIndex((s16) * (u16 *)(obj + 0xC) >> 8, (s16) * (u16 *)(obj + 0x10) >> 8);
    }

    if (cell == GA(u16, 0x2F4) || (*(u32 *)(gUnknown_02000580[0] + 0x20) & 0x20000)) {
        if (GA(u16, 0x2D4) & 0x400) {
            res = 0;
            goto after;
        }
        GB(u16, 0x40) &= ~4;
        if (GA(u16, 0x2D4) & 0x80) {
            Golem_SetControl(a, 0x1A, GB(void *, 0x48));
            Golem_SetState(a, 0, 0, 0xC);
        } else {
            Golem_SetControl(a, 1, GA(void *, 0x228));
            Golem_SetState(a, 0, 0, 5);
            pos = GB(u16 *, 0x200);
            src = *(u16 **)(base + 0x74 + 8);
            pos[0] = src[0];
            pos[2] = src[1];
            pos[1] = Golem_PosHeight(pos);
            pos = GB(u16 *, 0x200);
            pos[1] = Golem_PosHeight(pos);
            pos[3] = 0;
        }
        dir = GA(u8, 0x59);
        if (!((GA(u16, 0x2D4) & 0x400) || (GB(u16, 0x40) & 4))) {
            v = dir;
            for (i = 0; i <= 3; i++) {
                gRandomIndex = (gRandomIndex + 1) & 0x3FF;
                v = (gUnk_0203B400[gRandomIndex] & 3) << 6;
                if (v != (u32)dir)
                    break;
                v = dir;
            }
            GB(u8, 0x204) = v;
            sav = base + 0x8C;
            *(u32 *)sav = GA(u32, 0x21C);
            sav[7] = GA(u8, 0x2B5);
            sav[4] = GA(u8, 0x2A8);
            sav[5] = GA(u8, 0x2A9);
            sav[6] = GA(u8, 0x2AA);
            GB(u8, 0x205) = dir;
            Golem_SetControl(a, 0x1B, GB(void *, 0x21C));
            Golem_SetState(a, 3, 1, 0);
            GB(u8, 0x206) = 6;
        }
        sub_0807E334(a);
        GA(u16, 0x2C2) = 0x20;
        res = 0;
    } else {
        s32 rem = Mod(cell, *(s16 *)((u8 *)((u32 *)gTerrainMap)[1] + 4));
        s32 quo = Div(cell, *(s16 *)((u8 *)((u32 *)gTerrainMap)[1] + 4));
        q[0] = (rem << 8) + 0x80;
        {
            u8 *o = TerrainModifiers_FindCellOverride(cell, 1);
            u8 b;
            if (o)
                b = *(o + 4);
            else
                b = *((u8 *)((u32 *)gTerrainMap)[1] + cell * 4 + 0xC);
            q[1] = (b & 0xF) << 8;
        }
        q[2] = (quo << 8) + 0x80;
        if (variant & 1 ? Golem_ApplyBlocked2(a, GB(void *, 0x1FC), q)
                        : Golem_ApplyBlocked(a, GB(void *, 0x1FC), q)) {
            res = 1;
        } else {
        callback:
            Callback_InvokeActor(a, GB(void *, 0x224));
            res = 0;
        }
    }

after:
    if (res == 0) {
        if (!Golem_ApplyBlocked2(a, GB(void *, 0x1FC), GA(u8 *, 0xDC) + 0xC)) {
            Callback_InvokeActor(a, GB(void *, 0x224));
            return 0;
        }
    }
    return 1;
}

/* the prefix of the 0x80 / animation-B cases: a one-shot hit-react latch */
static inline void Golem_LatchHitReact(u8 *a)
{
    if (GA(u8, 0x5CA) == 0 && (GA(u16, 0x2E6) & 8)) {
        GA(u16, 0x2B8) = 0x2D;
        GA(u8, 0x5CA) = 0x41;
        SceneActorAnimation_SelectDirectionalEntry(a, 3);
    }
}

/* shared by the 0x100 and fall-through branches */
static inline void Golem_EnterCharge(u8 *a)
{
    GA(s16, 0xAA) = GA(s16, 0xA8);
    Golem_SetControl(a, 0x12, GA(void *, 0x230));
    Golem_SetState(a, 3, 0, 0);
    GA(u32, 0x2D0) &= 0xFEFFFFFF;
}

/* the common exit of the 0x2000 case and of the last Reposition copy */
static inline void Golem_Settle(u8 *a)
{
    Golem_SetControl(a, 1, GA(void *, 0x228));
    Golem_SetState(a, 0, 0, 7);
    GA(u16, 0x2C2) = 0x20;
}

#ifdef NONMATCHING
// fuzzy: 12.6%
// scope shape 2617 (differences once low registers are set aside)
// confidence=low: golem hit-reaction state machine; one flag cascade, four inlined Reposition copies, returns 1 if it entered a new state
s32 sub_080C9C04(u8 *a)
{
    u8 *ctx = GA(u8 *, 0x3D0);
    u8 *base = a + 0x3D4;
    u16 *flags = &GA(u16, 0x2D4);
    u32 fl;

    *(u32 *)(ctx + 0x628) |= 0x800;
    GA(u32, 0x2D0) &= 0xFEFFFFFF;
    SceneActorPlacement_PlayVariantEventSound(a, 1);

    if (*flags & 0x2000) {
        GA(u8, 0x59) = GA(u8, 0x2B6) + 0x80;
        SceneActorAnimation_SelectDirectionalEntry(a, 0);
        *flags &= 0xDFDF;
        GA(u32, 0x2CC) |= 0x10;
        GA(u16 *, 0x5D4)[3] = 0;
        GA(u8 *, 0xDC) = gUnknown_02000580[0] + 0x24;
        if (!Golem_Reposition(a, 0))
            return 0;
        Golem_Settle(a);
        return 1;
    }

    fl = *flags;
    if (fl & 0x8000) {
        *flags = fl & ~0x20;
        return 0;
    }
    if (fl & 2) {
        GA(u32, 0x318) = GA(u32, 0x5C);
        GA(u32, 0x31C) = GA(u32, 0x60);
        GA(u32, 0x310) = GA(u32, 0x5C);
        GA(u32, 0x314) = GA(u32, 0x60);
        Golem_SetControl(a, 7, GA(void *, 0x5DC));
        Golem_SetState(a, 3, 0, 0);
        return 1;
    }
    if (fl & 0x100) {
        if (GA(u32, 0x2D8) & 0x400) {
            GC(u16, 0x6AC) = 0x1E;
            GC(u16, 0x6AE) = 0x14;
            GA(s16, 0xAA) = GA(s16, 0xA8);
            Golem_SetControl(a, 0x12, GA(void *, 0x230));
            Golem_SetState(a, 3, 0, 0);
            GA(u32, 0x2CC) |= 0x80000;
            GA(u32, 0x2CC) &= ~0x80;
            GA(u32, 0x2D0) &= 0xFEFFFFFC;
            GA(u16, 0x2C2) = 0x40;
            return 1;
        }
        if (GA(u32, 0x2DC) & 0xC0000) {
            Golem_EnterCharge(a);
            return 1;
        }
        if (GA(u32, 0xDC)) {
            SceneActorRoute_CommitReachableDestination(a);
            GC(u16, 0x680) = 0x78;
        }
        *flags &= ~0x20;
        return 0;
    }
    if (GA(u32, 0x2DC) & 0xC0000) {
        Golem_EnterCharge(a);
        return 1;
    }

    *flags &= ~0x20;
    if (GA(u8, 0xDC + 0xF) & 0x10)
        return 0;

    if (*flags & 0x80) {
        Golem_LatchHitReact(a);
        if (!GA(u32, 0xDC))
            return 0;
        GA(u8 *, 0xDC) = gUnknown_02000580[0] + 0x24;
        GA(u32, 0x2CC) |= 0x80;
        GB(u16, 0x40) |= 4;
        if (!Golem_Reposition(a, 1))
            return 0;
        Golem_SetControl(a, 0x1A, GB(void *, 0x48));
        Golem_SetState(a, 0, 0, 0xC);
        return 1;
    }

    if (GC(u8, 0x624) == 0) {
        Golem_LatchHitReact(a);
        if (!GA(u32, 0xDC))
            return 0;
        GA(u8 *, 0xDC) = gUnknown_02000580[0] + 0x24;
        GA(u8, 0x59) = GA(u8, 0xE9);
        GB(u16 *, 0x200)[3] = 1;
        if (!Golem_Reposition(a, 2))
            return 0;
        GA(u16, 0x2C2) = 0x40;
        if (GC(u8, 0x624) <= 2)
            GC(u8, 0x624)++;
        Golem_SetControl(a, 0xA, SceneActorReaction_PlayAnimationBUntilComplete);
        Golem_SetState(a, 0, 1, 1);
        GA(u32, 0x2CC) |= 0x10;
        return 1;
    }

    Golem_LatchHitReact(a);
    if (!GA(u32, 0xDC))
        return 0;
    GA(u8 *, 0xDC) = gUnknown_02000580[0] + 0x24;
    GA(u32, 0x2CC) |= 0x90;
    GB(u16 *, 0x200)[3] = 0;
    if (!Golem_Reposition(a, 3))
        return 0;
    Golem_Settle(a);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080C9C04);
#endif

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

#ifdef NONMATCHING
// fuzzy: 77.7%
u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline s32 DelayedRecovery_SceneActorTerrain_Width(void)
{
    return *(s16 *)(gTerrainMap->cells + 4);
}
static inline u8 *DelayedRecovery_SceneActorTerrain_GetCell(s32 cell)
{
    u8 *override = TerrainModifiers_FindCellOverride(cell, 1);
    if (override)
        return override + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}
static inline void DelayedRecovery_SceneActorTerrain_GetCenter(s32 cell,
                                                               struct ContactPosition *position)
{
    s32 x = Mod(cell, DelayedRecovery_SceneActorTerrain_Width());
    s32 z = Div(cell, DelayedRecovery_SceneActorTerrain_Width());
    position->x = ((u32)x << 8) + 128;
    position->y = (*DelayedRecovery_SceneActorTerrain_GetCell(cell) & 15) << 8;
    position->z = ((u32)z << 8) + 128;
}

// Remaining: recovery stack slots, flag-address reuse and saved-state scheduling.
s32 Callback_InvokeActor(void *, void *);
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
void sub_0807E334(struct SceneActorRegistryResetView *);
u8 sub_0812F3CC(void *);
void SceneActorReaction_RestoreAfterDelay(void *);

void Sound_PlaySong(u32);

#define F8(a, o)  (*(u8 *)((u8 *)(a) + (o)))
#define F16(a, o) (*(u16 *)((u8 *)(a) + (o)))
#define S16(a, o) (*(s16 *)((u8 *)(a) + (o)))
#define F32(a, o) (*(u32 *)((u8 *)(a) + (o)))
#define PTR(a, o) (*(void **)((u8 *)(a) + (o)))

struct DelayedRecovery_GolemRoutePoint {
    s16 x, z;
};
struct DelayedRecovery_GolemRouteLink {
    u8 unknown0[8];
    struct DelayedRecovery_GolemRoutePoint *point;
};
struct DelayedRecovery_GolemFlags16 {
    u16 value;
};
#define HOME (*(struct ContactPosition **)(group + 0x200))
struct DelayedRecovery_GolemTurn {
    u8 direction, savedDirection, mode;
};
struct DelayedRecovery_GolemSavedControl {
    void *target;
    u8 state, phase, behavior, kind;
};

static inline u32 DelayedRecovery_Flag16(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u16 *)((u8 *)p + off) & mask;
}
static inline u32 DelayedRecovery_Flag32(void *p, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return *(u32 *)((u8 *)p + off) & mask;
}
static inline u8 DelayedRecovery_TestBits16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    if (*p & mask)
        return 1;
    return 0;
}
static inline s32 DelayedRecovery_PlayerSlot(u8 *actor)
{
    if (actor[8] <= 3)
        return actor[8];
    return -1;
}
static inline u8 DelayedRecovery_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void DelayedRecovery_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                              void *control)
{
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
static inline void DelayedRecovery_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                            u32 phase, u32 behavior)

{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 DelayedRecovery_MaskFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline u32 DelayedRecovery_CellIndex(struct ContactPosition *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u32 DelayedRecovery_CellHeight(u32 cell, struct ContactPosition *position)
{
    u32 shape, height;
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    shape = *data >> 4;
    height = (*data & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *((u8 *)position + 4);
    goto done;
slopeX:
    height -= *(u8 *)position;
done:
    return height;
}
static inline u8 DelayedRecovery_TryMove(struct SceneActorRegistryResetView *actor, void *route,
                                         struct ContactPosition *position,
                                         struct ContactPosition *destination)
{
    s32 result;
    if (!Nav_IsPositionInMesh(route, position, destination)) {
        u32 mask = ~0x200;
        actor->flags2D4 = mask & actor->flags2D4;
        return 0;
    } else {
        result = Terrain_ApplyBlocked(route, position, destination);
        if (result) {
            if (result != 2)
                Memory_Clear(&actor->motion, 8);
            return 1;
        } else {
            u32 mask = ~0x200;
            actor->flags2D4 = mask & actor->flags2D4;
            return 0;
        }
    }
}
static inline u32 DelayedRecovery_Random(const u16 *table, u32 *seed, u32 wrap, u32 bits)
{
    u32 index = (*seed + 1) & wrap;
    u32 v;
    *seed = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)table);
    v &= bits;
    return v;
}
static inline u32 DelayedRecovery_RandomDirectionExcept(u32 current)
{
    s32 i = 0;
    u32 d;
    do {
        d = DelayedRecovery_Random(gUnk_0203B400, &gRandomIndex, 0x3FF, 3) << 6;
        if (d != current)
            goto chosen;
    } while (++i <= 3);
    d = current;
chosen:
    return d;
}
static inline void DelayedRecovery_Wander(struct SceneActorRegistryResetView *input, u8 direction)
{
    struct SceneActorRegistryResetView *actor = input;
    u8 *group = (u8 *)&actor->group;
    if (!DelayedRecovery_Flag16(actor, 0x400, 0x2D4) &&
        !DelayedRecovery_TestBits16((u16 *)(group + 0x40), 4)) {
        ((struct DelayedRecovery_GolemTurn *)(group + 0x204))->direction =
            DelayedRecovery_RandomDirectionExcept(actor->direction);
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->target = actor->target;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->kind = actor->field2B5;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->state = actor->state2A8;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->phase = actor->phase2A9;
        ((struct DelayedRecovery_GolemSavedControl *)(group + 0x8C))->behavior = actor->behavior2AA;
        group[0x205] = direction;
        {
            void *target = PTR(group, 0x21C);
            DelayedRecovery_SetControl(actor, 0x1B, target);
        }
        DelayedRecovery_SetState(actor, 3, 1, 0);
        group[0x206] = 6;
    }
}
static inline u8 DelayedRecovery_Chase(struct SceneActorRegistryResetView *input)
{
    struct SceneActorRegistryResetView *actor;
    u8 *group;
    u32 cell;
    s32 slot;
    u32 *targetp;
    actor = input;
    group = (u8 *)&actor->group;
    targetp = &actor->fieldDC;
    {
        struct ContactPosition *dest = actor->state5D4;
        struct DelayedRecovery_GolemRouteLink *link = (void *)&actor->route;
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(dest), dest);
    }
    if (DelayedRecovery_Test(gEngineFlags, 0x4800)) {
        u8 *target = (u8 *)*targetp;
        if (!target)
            goto fail;
        slot = DelayedRecovery_PlayerSlot(target);
        if (slot < 0)
            goto fail;
        cell = *(u16 *)((u8 *)PTR(gPlayers[slot], 0x40) + 6);
    } else {
        cell = DelayedRecovery_CellIndex((struct ContactPosition *)(*targetp + 0xC));
        slot = 0;
    }
    if (cell == actor->terrainCell || DelayedRecovery_Flag32(gPlayers[slot], 0x20000, 0x20)) {
        u16 busy = DelayedRecovery_Flag16(actor, 0x400, 0x2D4);
        u16 blocked;
        if (busy)
            return 0;
        {
            struct DelayedRecovery_GolemFlags16 *flags = (void *)(group + 0x40);
            s32 mask = ~4;
            flags->value = mask & flags->value;
        }
        blocked = DelayedRecovery_Flag16(actor, 0x80, 0x2D4);
        if (blocked) {
            void *target = PTR(group, 0x48);
            DelayedRecovery_SetControl(actor, 0x1A, target);
            DelayedRecovery_SetState(actor, 0, 0, 12);
        } else {
            {
                void *target = actor->entryTarget228;
                DelayedRecovery_SetControl(actor, 1, target);
            }
            DelayedRecovery_SetState(actor, 0, 0, 5);
            {
                struct ContactPosition *dest = *(struct ContactPosition **)(group + 0x200);
                struct DelayedRecovery_GolemRouteLink *link =
                    (struct DelayedRecovery_GolemRouteLink *)(group + 0x74);
                dest->x = link->point->x;
                dest->z = link->point->z;
                dest->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(dest), dest);
                HOME->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(HOME), HOME);
                HOME->pad = 0;
            }
        }
        DelayedRecovery_Wander(input, actor->direction);
        sub_0807E334(input);
        actor->field2C2 = 0x20;
        return 0;
    }
    {
        struct ContactPosition center;
        DelayedRecovery_SceneActorTerrain_GetCenter(cell, &center);
        if (!DelayedRecovery_TryMove(input, PTR(group, 0x1FC), &actor->position, &center)) {
        fail:
            Callback_InvokeActor(actor, PTR(group, 0x224));
            return 0;
        }
    }
    return 1;
}

struct DelayedRecovery_DelayedRoute {
    u8 pad0[8];
    u8 state, phase, behavior, kind;
    u32 delay;
    void *resume;
};
static inline void DelayedRecovery_PrepareDelay(u8 *group, void *callback, u32 behavior)
{
    struct DelayedRecovery_DelayedRoute *saved = (void *)(group + 0x8c);
    saved->delay = 30;
    saved->resume = callback;
    saved->state = 0;
    saved->phase = 0;
    saved->behavior = behavior;
}
// confidence=low: waits for a recovery delay, then restores chase or return-home movement.
void Golem_ResumeDelayedRoute(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group;
    u32 zero;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->update270))
        return;
    zero = (u8)sub_0812F3CC(actor);
    if (zero)
        return;
    if ((s32)actor->counter2AC <= 60)
        goto waiting;
    SceneActorAttachment_ReplaceResponseSlot((u8 *)actor, 0, 2, actor->category);
    DelayedRecovery_SetState(actor, 3, 1, zero);
    DelayedRecovery_SetControl(actor, 14, SceneActorReaction_RestoreAfterDelay);
    if (DelayedRecovery_MaskFlags(actor, 0x80)) {
        DelayedRecovery_PrepareDelay(group, *(void **)(group + 0x48), 12);
        DelayedRecovery_Chase(actor);
    } else {
        struct ContactPosition *dest;
        struct DelayedRecovery_GolemRouteLink *link;
        actor->flags2CC |= 0x10;
        DelayedRecovery_PrepareDelay(group, actor->entryTarget228, 7);
        dest = HOME;
        link = (void *)(group + 0x74);
        dest->x = link->point->x;
        dest->z = link->point->z;
        dest->y = DelayedRecovery_CellHeight(DelayedRecovery_CellIndex(dest), dest);
        if (!DelayedRecovery_TryMove(actor, PTR(group, 0x1fc), &actor->position, HOME)) {
            DelayedRecovery_SetControl(actor, 0, SceneActorControl_OnIdle);
            DelayedRecovery_SetState(actor, 3, 0, 0);
        } else
            HOME->pad = 1;
    }
    return;
waiting:
    actor->counter2AC++;
}

#else
INCLUDE_ASM("asm/nonmatching", Golem_ResumeDelayedRoute);
#endif

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
    Golem_UpdateAttackProperties(actor);
    return 1;
}
