#include "game_state.h"
#include "scene_actor_behavior.h"
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
 * Kill counters are an if/else (its two stores raise the counter
 * pointer above the reaction flags; cross-jumping merges them). The random table is read through
 * its literal address (REGALLOC.md, "Reload rotation"); the CpuSet source shares a struct
 * with the script values so that it takes the first stack slot. */

void *Bestiary_GetEntryForActor(u32, u32);
void Bestiary_UnlockEntry(void *);
void SceneActorBehavior_CallTerrainScript(void *);
void SceneActorPlacement_PlayCategoryEventSound(void *);
#include "scene_actor_part_burst.h"
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
static inline u32 Flag(struct ChaseActor *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline void SetCtl(struct ChaseActor *actor, void *cb, u32 k)
{
    void *callback = cb;
    u32 kind = k;
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *field = &actor->base.flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *actor, u32 s, u32 p, u32 b)
{
    u32 state = s;
    actor->base.state2A8 = state;
    actor->base.phase2A9 = p;
    actor->base.behavior2AA = b;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
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
static inline void RecordKill(struct ChaseActor *actor, u8 *category)
{
    struct ContactCollider *collider = &actor->base.colliderA;
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
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
static inline void ClearPos(void *dest, u32 *zero)
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

void Ivy_UpdateDefeat(struct ChaseActor *actor, s32 frame)
{
    struct {
        u32 zero;
        s32 values[8];
    } buf;
    struct DefeatContext *context = actor->base.context;
    u32 *drawable = actor->base.drawable;
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
            actor->base.flags2D4 = m | actor->base.flags2D4;
        }
        SceneActorAnimation_SelectDirectionFlagsVariant((void *)actor, 10, 1);
        {
            struct ContactCollider *collider = &actor->base.colliderA;
            {
                u32 flags = collider->flags;
                flags |= 4;
                flags &= 0xFFF7;
                collider->flags = flags;
            }
        }
        {
            u32 m = 0x800;
            actor->base.flags2C0 = m | actor->base.flags2C0;
        }
        SceneActorPlacement_PlayCategoryEventSound(actor);
        ClearPos(&vec, &buf.zero);
        Polar(&vec, actor->base.reactionDirection, Rand() & 0x1F);
        vec.y = (Rand() & 0x1F) + 0x30;
        sub_08127590(&actor->base.body.position, &vec, 0xD638, 0x99DD, 10, 0x1E, 2, 0, 0x12A, 0, 0,
                     0, 0, 0, 0);
        hidden = actor->base.field2DC & 0x10;
        if (hidden)
            Sound_PlaySong(0xE5);
        else {
            u32 *script = (u32 *)((u8 *)context + 0x638);
            if (*script) {
                single = actor->base.body.id;
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
        u32 hidden = actor->base.field2DC & 0x10;
        if (!hidden) {
            s32 h = actor->base.defaultHealth;
            if (h <= 0)
                h = 0;
            actor->base.health = h;
            {
                u32 bit = 0x40;
                actor->base.flags2D4 &= ~bit;
            }
            SetState(actor, 0, 0, 1);
            SetCtl(actor, (void *)SceneActorReaction_PlayHiddenCue, 0x2A);
            {
                struct ContactCollider *collider = &actor->base.colliderA;
                {
                    u32 bit = 4;
                    collider->flags = (collider->flags | 8 | hidden) & ~bit;
                }
            }
            {
                u32 bit = 0x800;
                actor->base.flags2C0 &= ~bit;
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
            actor->base.flags2D6 = m | actor->base.flags2D6;
        }
        categoryp = &actor->base.category;
    } else {
        struct ContactPackedVector range;
        const u16 *table;
        union HitSparkTable source = {0};
        u8 category;
        range.x = 80;
        range.y = 60;
        range.z = 60;
        source.encoded.offset = (u16)RANDOM_TABLE_ADDR;
        source.encoded.page = (u8)(RANDOM_TABLE_ADDR >> 16);
        source.encoded.region = (u8)(RANDOM_TABLE_ADDR >> 24);
        table = ReadSparkTable(&source);
        {
            u8 firstCount = SceneActorEffects_NextSparkCount(table);
            SceneActorEffects_SpawnDefeatSparks(actor, (const struct ContactPosition *)&range,
                                                table, firstCount);
        }
        category = actor->base.category;
        categoryp = &actor->base.category;
        if (category == 4 || category == 1 || category == 5 || category == 7 || category == 6 ||
            category == 0x18 || category == 0x19)
            Sound_PlaySong(0xE5);
    }
    *drawable |= 1;
    SceneActorBehavior_CallTerrainScript(actor);
    {
        u16 script = (u16)actor->base.script1F8;
        if (script) {
            struct ScriptUtilityArguments arguments;
            buf.values[0] = actor->base.body.position.x;
            buf.values[1] = actor->base.body.position.y;
            buf.values[2] = actor->base.body.position.z;
            buf.values[3] = actor->base.parameters1FC[0];
            buf.values[4] = actor->base.parameters1FC[1];
            buf.values[5] = actor->base.parameters1FC[2];
            buf.values[6] = actor->base.field2E0;
            buf.values[7] = actor->base.body.id;
            arguments.count = 8;
            arguments.values = buf.values;
            Script_Call(script, &arguments);
        }
    }
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(*categoryp - 11) > 2)
        Bestiary_UnlockEntry(Bestiary_GetEntryForActor(*categoryp, actor->base.paletteOffset));
    SetCtl(actor, (void *)SceneActorControl_OnIdle, 16);
    SetState(actor, 1, 0, 2);
    COUNT(0x87C);
    COUNT(0x538);
    RecordKill(actor, categoryp);
    sub_08159164(actor->base.fieldAC);
    Player_RefreshLifeState(actor->base.field2D8, *((u8 *)actor + 0xA1));
    if (!Test(gEngineFlags, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, (u16)actor->base.body.id);
}
