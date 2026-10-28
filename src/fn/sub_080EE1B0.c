// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "script_utility.h"
#include "engine.h"
#include "runtime.h"

/* Ivy defeat state (tentative), a variant of Skeleton_UpdateDefeat (Skeleton_UpdateDefeat). On entry:
 * hides the actor, launches a particle burst through sub_08127590 and either plays cue 0xE5
 * (context flag 0x10) or runs the context's defeat script. After 20 frames, without flag 0x10
 * it revives (restores health, SceneActorReaction_PlayHiddenCue); otherwise the full defeat:
 * cue or six-spark burst, terrain and position scripts, event flag, kill counters per
 * category (saturating at 9999, plus one counter per collider reaction flag) and the reward.
 * Pin: the spark table. Kill counters are an if/else (its two stores raise the counter
 * pointer above the reaction flags; cross-jumping merges them). The random table is read through
 * its literal address (REGALLOC.md, "Reload rotation"); the CpuSet source shares a struct
 * with the script values so that it takes the first stack slot. */

void *BestiaryDisplay_GetEntryForActor(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
void sub_08127590(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void sub_08159164(u32);
void Player_RefreshLifeState(u32, u32);
void SceneActorRecord_RecordPair(u32, u32);

#define STAT(n)  (*(s16 *)((u8 *)gGameState + (n)))
#define COUNT(n) Count(&STAT(n))
#define INC(n)   STAT(n) = Clamp(STAT(n) + 1)

static inline void Count(s16 *p)
{
    if (*p < 9999)
        *p = *p + 1;
    else
        *p = 9999;
}
static inline s32 Clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Masked16(u16 *p, u32 maskInput)
{
    u32 mask = maskInput;
    return *p & mask;
}
static inline u8 TestBits(u32 *p, u32 maskInput)
{
    u32 mask = maskInput;
    if (*p & mask)
        return 1;
    return 0;
}
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline void SetCtl(struct SceneActorRegistryResetView *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 s, u32 p, u32 b)
{
    u32 state = s;
    actor->state2A8 = state;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 RandomFrom(const u16 *table, u32 wrap, u32 bits)
{
    u32 index = (gRandomIndex + 1) & wrap;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)(index + (u32)table);
    v &= bits;
    return v + 1;
}
static inline void Polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * radius);
}
#define FLAG_COUNTS()                                                                              \
    {                                                                                              \
        if (flags & 0x80)                                                                          \
            INC(0x58A);                                                                            \
        else if (flags & 0x100)                                                                    \
            INC(0x58C);                                                                            \
        else if (flags & 0x200)                                                                    \
            INC(0x58E);                                                                            \
        else if (flags & 0x400)                                                                    \
            INC(0x590);                                                                            \
        else if (flags &= 0x800)                                                                   \
            INC(0x592);                                                                            \
    }
static inline void RecordKill(struct SceneActorRegistryResetView *actor, u8 *category)
{
    struct ContactCollider *collider = &actor->colliderA;
    switch (*category) {
        case 4: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53A);
            FLAG_COUNTS();
            break;
        }
        case 1: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53C);
            FLAG_COUNTS();
            break;
        }
        case 2: {
            u32 flags = collider->reactionFlags;
            COUNT(0x53E);
            FLAG_COUNTS();
            break;
        }
        case 3: {
            u32 flags = collider->reactionFlags;
            COUNT(0x540);
            FLAG_COUNTS();
            break;
        }
        case 14: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54C);
            FLAG_COUNTS();
            break;
        }
        case 24: {
            u32 flags = collider->reactionFlags;
            COUNT(0x55C);
            FLAG_COUNTS();
            break;
        }
        case 10: {
            u32 flags = collider->reactionFlags;
            COUNT(0x546);
            FLAG_COUNTS();
            break;
        }
        case 5:
        case 6:
        case 7: {
            u32 flags = collider->reactionFlags;
            COUNT(0x542);
            FLAG_COUNTS();
            break;
        }
        case 21: {
            u32 flags = collider->reactionFlags;
            COUNT(0x554);
            FLAG_COUNTS();
            break;
        }
        case 18: {
            u32 flags = collider->reactionFlags;
            COUNT(0x550);
            FLAG_COUNTS();
            break;
        }
        case 19: {
            u32 flags = collider->reactionFlags;
            COUNT(0x552);
            FLAG_COUNTS();
            break;
        }
        case 17: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54E);
            FLAG_COUNTS();
            break;
        }
        case 22: {
            u32 flags = collider->reactionFlags;
            COUNT(0x556);
            FLAG_COUNTS();
            break;
        }
        case 20: {
            u32 flags = collider->reactionFlags;
            COUNT(0x558);
            FLAG_COUNTS();
            break;
        }
        case 9: {
            u32 flags = collider->reactionFlags;
            COUNT(0x544);
            FLAG_COUNTS();
            break;
        }
        case 23: {
            u32 flags = collider->reactionFlags;
            COUNT(0x55A);
            FLAG_COUNTS();
            break;
        }
        case 12:
        case 13: {
            u32 flags = collider->reactionFlags;
            COUNT(0x54A);
            FLAG_COUNTS();
            break;
        }
        case 25: {
            u32 flags = collider->reactionFlags;
            COUNT(0x560);
            FLAG_COUNTS();
            break;
        }
    }
}

void SceneActorReaction_PlayHiddenCue(void);
static inline u8 SaveFlag(u32 maskInput)
{
    u32 mask = maskInput;
    if (*(u32 *)((u8 *)gGameState + 0x868) & mask)
        return 1;
    return 0;
}
static inline void ClearPos(void *dest, vu32 *zero)
{
    *zero = 0;
    CpuSet((void *)zero, dest, 0x05000002);
}
static inline u16 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(index + RANDOM_TABLE_ADDR);
}

struct DefeatContext {
    u8 unknown0[0x638];
    u16 script;
};

void Ivy_UpdateDefeat(struct SceneActorRegistryResetView *actor, s32 frame)
{
    struct {
        u32 zero;
        s32 values[8];
    } buf;
    struct DefeatContext *context = actor->context;
    u32 *drawable = actor->drawable;
    u8 *categoryp;
    u32 wrap;
    u32 bits;
    s32 count;
    s32 life;
    s32 size;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct ContactPosition vec;
        s32 single;
        struct ScriptUtilityArguments arguments;
        u32 hidden;
        {
            u16 m = 0x40;
            actor->flags2D4 = m | actor->flags2D4;
        }
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 10, 1);
        {
            struct ContactCollider *collider = &actor->colliderA;
            {
                u32 flags = collider->flags;
                flags |= 4;
                flags &= 0xFFF7;
                collider->flags = flags;
            }
        }
        {
            u32 m = 0x800;
            actor->flags2C0 = m | actor->flags2C0;
        }
        SceneActorPlacement_PlayCategoryEventSound(actor);
        ClearPos(&vec, (vu32 *)&buf.zero);
        Polar(&vec, actor->field2B6, Rand() & 0x1F);
        vec.y = (Rand() & 0x1F) + 0x30;
        sub_08127590(&actor->position, &vec, 0xD638, 0x99DD, 10, 0x1E, 2, 0, 0x12A, 0, 0, 0, 0, 0,
                     0);
        hidden = actor->field2DC & 0x10;
        if (hidden)
            Sound_PlaySong(0xE5);
        else {
            u32 *script = (u32 *)((u8 *)context + 0x638);
            if (*script) {
                single = actor->id;
                arguments.count = 1;
                arguments.values = &single;
                Script_Call(*script, &arguments);
                *script = hidden;
            }
        }
    }
    if (frame <= 0x14)
        return;
    {
        u32 hidden = actor->field2DC & 0x10;
        if (!hidden) {
            s32 h = actor->defaultHealth;
            if (h <= 0)
                h = 0;
            actor->health = h;
            {
                u32 bit = 0x40;
                actor->flags2D4 &= ~bit;
            }
            SetState(actor, 0, 0, 1);
            SetCtl(actor, (void *)SceneActorReaction_PlayHiddenCue, 0x2A);
            {
                struct ContactCollider *collider = &actor->colliderA;
                {
                    u32 bit = 4;
                    collider->flags = (collider->flags | 8 | hidden) & ~bit;
                }
            }
            {
                u32 bit = 0x800;
                actor->flags2C0 &= ~bit;
            }
            return;
        }
    }
    wrap = 1023;
    bits = 3;
    count = 4;
    life = 20;
    if (SaveFlag(0x400)) {
        Sound_PlaySong(0xE5);
        {
            u32 m = 0x1000;
            actor->flags2D6 = m | actor->flags2D6;
        }
        categoryp = &actor->category;
    } else {
        struct ContactPackedVector range;
        register const u16 *table asm("sb");
        u8 category;
        range.x = 80;
        range.y = 60;
        range.z = 60;
        table = gRandomTable;
        SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x20, count,
                            life, size = 2, 1);
        SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x40, count,
                            life, size, 1);
        SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x5E, count,
                            life, size, 1);
        SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0x80, count,
                            life, size, 1);
        SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0xC4, count,
                            life, size, 1);
        SparkParticle_Spawn(&actor->position, RandomFrom(table, wrap, bits), &range, 0xF0, count,
                            life, size, 1);
        category = actor->category;
        categoryp = &actor->category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    *drawable |= 1;
    SceneActorBehavior_CallTerrainScript(actor);
    {
        u16 script = *(u16 *)((u8 *)actor + 0x1F8);
        if (script) {
            struct ScriptUtilityArguments arguments;
            buf.values[0] = actor->position.x;
            buf.values[1] = actor->position.y;
            buf.values[2] = actor->position.z;
            buf.values[3] = *(s32 *)((u8 *)actor + 0x1FC);
            buf.values[4] = *(s32 *)((u8 *)actor + 0x200);
            buf.values[5] = *(s32 *)((u8 *)actor + 0x204);
            buf.values[6] = actor->field2E0;
            buf.values[7] = actor->id;
            arguments.count = 8;
            arguments.values = buf.values;
            Script_Call(script, &arguments);
        }
    }
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(*categoryp - 11) > 2)
        GameState_SetEventFlag(BestiaryDisplay_GetEntryForActor(*categoryp, actor->paletteOffset));
    SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
    SetState(actor, 1, 0, 2);
    COUNT(0x87C);
    COUNT(0x538);
    RecordKill(actor, categoryp);
    sub_08159164(*(u32 *)((u8 *)actor + 0xAC));
    Player_RefreshLifeState(actor->field2D8, *((u8 *)actor + 0xA1));
    if (!Test(gEngineFlags, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->id);
}
