#include "global.h"
#ifdef NONMATCHING
// fuzzy: 98.9%
// MATCHING_CONCESSION: register-pinning
#include "scene_actor_reaction.h"
#include "script_utility.h"
s32 sub_0806A160(const struct ContactPosition *, u8, const struct ContactPosition *, s32, s32, s32,
                 s32, u32);
void SceneActorPlacement_PlayCategoryEventSound(void *);
void sub_0812E714(void *);
void *sub_081D4F20(u32, u32);
void GameState_SetEventFlag(void *);
void SceneActorRecord_RecordPair(u16, u16);
extern u16 gUnk_030039F0;
static inline u32 NextRandom(const u16 *table)
{
    u32 *index = &gUnk_03005308;
    u32 next;
    asm("" : "+r"(index));
    next = (*index + 1) & 1023;
    *index = next;
    return table[next];
}
static inline u32 FirstRandom(const u16 *table)
{
    register u32 *index asm("r3") = &gUnk_03005308;
    u32 next;
    next = (*index + 1) & 1023;
    *index = next;
    return table[next];
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 TakePrepared(struct SceneActorRegistryResetView *actor)
{
    u32 offset = 0x2B2;
    u8 *field;
    field = (u8 *)actor + offset;
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}
static inline u32 Flag(struct SceneActorRegistryResetView *actor, u32 maskInput, u32 off)
{
    u32 mask = maskInput;
    return off == 0x2D4 ? *(u16 *)((u8 *)actor + off) & mask : *(u32 *)((u8 *)actor + off) & mask;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor)
{
    void *callback = (void *)sub_0807FC90;
    u8 kind = 16;
    register u32 zero asm("r3");
    {
        register u32 preparedOffset asm("r4") = 0x2B2;
        u8 *field;
        register u8 one asm("r4");
        asm("" : "+r"(preparedOffset));
        field = (u8 *)actor + preparedOffset;
        asm("" : "+r"(field));
        zero = 0;
        asm("" : "+r"(zero));
        one = 1;
        *field = one;
    }
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = callback;
    {
        u32 off = 0x2C8;
        {
            u16 *field = (void *)((u8 *)actor + off);
            *field = zero;
        }
    }
    {
        u32 *field = &actor->flags114;
        u32 mask = ~1;
        *field &= mask;
    }
    actor->word104 = zero;
}
void SceneActorReaction_BeginDefeatBurst(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorReactionGroup *group = (void *)&actor->group;
    struct ContactPosition range;
    s32 values[8];
    struct ScriptUtilityArguments arguments;
    u16 script;
    void *drawable;
    const u16 *table;
    if (!TakePrepared(actor))
        return;
    {
        struct SceneActorRegistryControl *control = &group->control;
        u32 offset = 0x2CC;
        u32 *field;
        u32 mask;
        u32 zero;
        field = (void *)((u8 *)actor + offset);
        mask = ~0x141C0;
        *field &= mask;
        zero = 0;
        control->counter = zero;
        *(u16 *)((u8 *)control + 0x14) = zero;
        control->timer = zero;
        {
            struct ContactCollider *collider;
            u32 colliderOffset = 0x134;
            register u32 old asm("r2");
            u32 bit;
            collider = (void *)((u8 *)actor + colliderOffset);
            old = collider->flags;
            bit = 4;
            collider->flags = bit | old;
            {
                register u32 parameterOffset asm("r2") = 0x178;
                asm("" : "+r"(parameterOffset));
                {
                    register u16 *field asm("r0") = (void *)((u8 *)actor + parameterOffset);
                    *field = zero;
                }
            }
            actor->fieldAA = zero;
        }
    }
    {
        u32 showFlag = 1;
        if (actor->drawableKind == 0) {
            u32 *p = actor->drawable;
            *p |= showFlag;
            drawable = p;
        } else {
            u8 *p = actor->drawable;
            u8 *tile = p + 0x20;
            *(u32 *)(tile + 8) |= showFlag;
            drawable = p;
        }
    }
    if (actor->drawableKind == 0) {
        u32 mask = ~2;
        *(u32 *)drawable &= mask;
    } else {
        u8 *tile = (u8 *)drawable + 0x20;
        u32 mask = ~2;
        *(u32 *)(tile + 8) &= mask;
    }
    {
        u32 blend = 2;
        if (actor->drawableKind == 0)
            *((u8 *)drawable + 7) = blend;
        else {
            u8 *field = (u8 *)drawable + 0x3A;
            *field = blend;
        }
    }
    range.x = 80;
    range.y = 60;
    range.z = 60;
    {
        register struct ContactPosition *rangePointer asm("r6") = &range;
        u32 life;
        u32 mode;
        register u32 flag asm("r8");
        {
            register const u16 *raw asm("r4") = gUnk_0203B400;
            asm("" : : "r"(raw));
            table = raw;
        }
        asm("" : "+r"(table));
        sub_0806A160(&actor->position, (FirstRandom(table) & 3) + 1, rangePointer, 32, 4,
                     (life = 20), (mode = 2), (flag = 1));
        sub_0806A160(&actor->position, (NextRandom(table) & 3) + 1, rangePointer, 64, 4, life, mode,
                     flag);
        sub_0806A160(&actor->position, (NextRandom(table) & 3) + 1, rangePointer, 94, 4, life, mode,
                     flag);
        sub_0806A160(&actor->position, (NextRandom(table) & 3) + 1, rangePointer, 128, 4, life,
                     mode, flag);
        sub_0806A160(&actor->position, (NextRandom(table) & 3) + 1, rangePointer, 196, 4, life,
                     mode, flag);
        sub_0806A160(&actor->position, (NextRandom(table) & 3) + 1, rangePointer, 240, 4, life,
                     mode, flag);
    }
    if (actor->category == 4 || actor->category == 1 || actor->category == 5 ||
        actor->category == 7 || actor->category == 6 || actor->category == 24 ||
        actor->category == 25)
        Sound_PlaySong(229);
    SceneActorPlacement_PlayCategoryEventSound(actor);
    if (actor->category == 3)
        sub_0812E714(actor);
    script = *(u16 *)((u8 *)actor + 0x1F8);
    if (script) {
        values[0] = actor->position.x;
        values[1] = actor->position.y;
        values[2] = actor->position.z;
        values[3] = *(s32 *)((u8 *)actor + 0x1FC);
        values[4] = *(s32 *)((u8 *)actor + 0x200);
        values[5] = *(s32 *)((u8 *)actor + 0x204);
        values[6] = actor->field2E0;
        values[7] = actor->id;
        arguments.count = 8;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
    if (!Flag(actor, 0x4000, 0x2D4) && (u32)(actor->category - 11) > 2)
        GameState_SetEventFlag(sub_081D4F20(actor->category, actor->paletteOffset));
    SetControl(actor);
    {
        register u8 behavior asm("r1") = 2;
        register u32 offset asm("r2") = 0x2A8;
        asm("" : "+r"(behavior));
        {
            u8 *field = (u8 *)actor + offset;
            register u8 one asm("r4");
            asm("" : "+r"(field));
            one = 1;
            *field = one;
        }
        offset += 1;
        *((u8 *)actor + offset) = 0;
        {
            register u32 behaviorOffset asm("r4") = 0x2AA;
            *((u8 *)actor + behaviorOffset) = behavior;
            actor->counter2AC = 0;
            offset += 8;
            {
                register u8 *field asm("r0") = (u8 *)actor + offset;
                register u8 one asm("r3");
                one = 1;
                *field = one;
            }
            asm("" : : "r"(behaviorOffset), "r"(behavior));
        }
    }
    if (!Test(gUnk_030053F4, 0x4800) && !Flag(actor, 0x800, 0x2D0))
        SceneActorRecord_RecordPair(gUnk_030039F0, actor->id);
    ActorShadow_Disable((u8 *)actor + 0x320);
}

#else
INCLUDE_ASM("asm/nonmatching", SceneActorReaction_BeginDefeatBurst);
#endif
