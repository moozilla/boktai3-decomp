#include "scene_actor_chase_shared.h"
#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_reaction.h"
#include "runtime.h"
/* At zero health: hand control to the defeat target (kind 17), then bump the saturating defeat counters in the game-state block by category and weapon kind. */
void SceneActorRecord_RecordPair(u16, u16);
void SceneActorContacts_ReleaseNode(void *);
void Player_NotifyFirstSelectedSlot(u32, u32);
void Player_RefreshLifeState(u32, u32);
void sub_08159164(u32);
#define STATS   ((u8 *)gGameState)
#define H(p, n) (*(s16 *)((p) + (n)))
struct GroupLink {
    u32 unknown0;
    u32 flags;
};
struct FlagHalfword {
    s16 bits : 16;
};
static inline void ClearHit(struct FlagHalfword *flags, u32 mask)
{
    flags->bits = mask & flags->bits;
}
static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
/* Saturating defeat counters in the game-state block. */
#define STAT_INC(n) Count(&H(STATS, n))
#define STAT_ADD(n) H(STATS, n) = clamp(H(STATS, n) + 1)
#define KILL_KIND(flags)                                                                           \
    {                                                                                              \
        if ((flags) & 0x80)                                                                        \
            STAT_ADD(0x58A);                                                                       \
        else if ((flags) & 0x100)                                                                  \
            STAT_ADD(0x58C);                                                                       \
        else if ((flags) & 0x200)                                                                  \
            STAT_ADD(0x58E);                                                                       \
        else if ((flags) & 0x400)                                                                  \
            STAT_ADD(0x590);                                                                       \
        else if ((flags) &= 0x800)                                                                 \
            STAT_ADD(0x592);                                                                       \
    }
#define CASE_BODY(n)                                                                               \
    {                                                                                              \
        u32 flags = hit->reactionFlags;                                                            \
        STAT_INC(n);                                                                               \
        KILL_KIND(flags);                                                                          \
    }
static inline void Count(s16 *p)
{
    if (*p < 9999)
        *p = *p + 1;
    else
        *p = 9999;
}
static inline u32 Mask(u16 *flags, u32 maskInput)
{
    u32 mask = maskInput;
    return *flags & mask;
}
static inline u8 Test2(u16 **out, struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    u16 *flags = &actor->base.placementFlags;
    u32 value = *flags & mask;
    *out = flags;
    if (value)
        return 1;
    return 0;
}
static inline u32 Flag2D0(struct ChaseActor *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->base.statusFlags & mask;
}
u32 SceneActorReaction_RecordDefeat(struct ChaseActor *actor)
{
    u16 *flags2C0;
    u8 *category;
    struct ContactCollider *collider;
    struct ContactCollider *hit;
    u32 kind;
    if (actor->base.health <= 0) {
        {
            struct FlagHalfword *flags = (void *)&actor->base.reactionFlags;
            ClearHit(flags, ~0x49CB);
        }
        {
            void *control = actor->base.onDefeat;
            SceneActorTerrain_InstallControl(actor, control, 17);
        }
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        if (SceneActorChase_Test(gEngineFlags, 0x4800)) {
            Player_NotifyFirstSelectedSlot(actor->base.hitOwners, actor->base.hitAttributes);
            goto done;
        }
        {
            u8 *address = &actor->base.category;
            u32 owner;
            kind = *address;
            owner = actor->base.hitOwners & 1;
            category = address;
            flags2C0 = &actor->base.placementFlags;
            collider = &actor->base.colliderA;
            if (!owner)
                goto record;
        }
        {
            u8 blocked;
            flags2C0 = &actor->base.placementFlags;
            blocked = SceneActorChase_Test(Mask(flags2C0, 0x80), ~0);
            collider = &actor->base.colliderA;
            if (blocked)
                goto record;
        }
        {

            if (kind != 5 && kind != 7 && kind != 6) {
                STAT_INC(0x87C);
                STAT_INC(0x538);
                hit = &actor->base.colliderA;
                {
                    u32 index = *category - 1; /* category 1..25 */
                    collider = hit;
                    switch (index) {
                        case 3:
                            CASE_BODY(0X53A);
                            break;
                        case 0:
                            CASE_BODY(0X53C);
                            break;
                        case 1:
                            CASE_BODY(0X53E);
                            break;
                        case 2:
                            CASE_BODY(0X540);
                            break;
                        case 13:
                            CASE_BODY(0X54C);
                            break;
                        case 23:
                            CASE_BODY(0X55C);
                            break;
                        case 9:
                            CASE_BODY(0X546);
                            break;
                        case 4:
                        case 5:
                        case 6:
                            CASE_BODY(0X542);
                            break;
                        case 20:
                            CASE_BODY(0X554);
                            break;
                        case 17:
                            CASE_BODY(0X550);
                            break;
                        case 18:
                            CASE_BODY(0X552);
                            break;
                        case 16:
                            CASE_BODY(0X54E);
                            break;
                        case 21:
                            CASE_BODY(0X556);
                            break;
                        case 19:
                            CASE_BODY(0X558);
                            break;
                        case 8:
                            CASE_BODY(0X544);
                            break;
                        case 22:
                            CASE_BODY(0X55A);
                            break;
                        case 11:
                        case 12:
                            CASE_BODY(0X54A);
                            break;
                        case 24:
                            CASE_BODY(0X560);
                            break;
                    }
                }
            }
            sub_08159164(actor->base.experience);
            Player_RefreshLifeState(actor->base.hitAttributes, actor->base.fieldA1);
        }
    record:
        if (!SceneActorChase_Test(Mask(flags2C0, 0x80), ~0) &&
            !SceneActorChase_Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->base.body.id);
        ((struct ContactCollider *)collider)->flags |= 4;
    done:
        return 1;
    }
    return (u8)Callback_InvokeActor(actor, actor->base.onContact);
}
