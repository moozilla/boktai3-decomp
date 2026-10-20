#include "global.h"
#include "engine.h"
#include "player.h"
#include "contact_actor.h"

struct PlayerSelection;

void sub_08220204(struct ContactCollider *, struct ContactCollider *);
void sub_08238580(struct Player *, struct ContactCollider *, struct ContactCollider *);
s32 LinkSession_ClearFirstPlayerKind(void *, s32);
void LinkPlayer_FlushPendingEffects(struct Player *);
void sub_0813D548(struct Player *, s32);
void sub_0813D834(struct Player *, s32, s32);
s32 SceneParticle_CreateDefaultPriority(const struct ContactPosition *,
                                        const struct ContactPosition *,
                                        const struct ContactPosition *,
                                        const struct ContactPosition *, s32);
struct Player *Player_FindMaskedSlot(const struct PlayerSelection *);
void sub_08242FAC(struct Player *, u32);
void sub_08243020(struct Player *, u32);

static inline u32 Test(u32 m, u32 *pv)
{
    return *pv & m;
}

static inline u32 Test16(u32 m, u16 *pv)
{
    return *pv & m;
}

static inline u32 InMode(struct Player *p, u32 mode)
{
    u32 r;
    u16 a;
    if (!p)
        return 0;
    a = p->active;
    r = 0;
    if (a != 0) {
        if (p->sceneMode == mode)
            r = 1;
    }
    return r;
}

static inline s32 TruncShift(s32 value, s32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}

u32 Player_ApplyContact(struct Player *p, struct ContactCollider *source,
                        struct ContactCollider *target)
{
    u32 amount;
    u32 shift;

    {
        s32 count = source->unknown3C;
        if (count > 0)
            sub_08220204(source, target);
        else
            target->cost = 0;
    }
    target->responseValue = source->responseValue;
    shift = 0;
    if (source->responseParameter == 0) {
        p->contactActionTag = shift;
    } else {
        p->contactActionTag = 0xE6;
        if (p->actionMode == 4 && p->tetherMode == 0xE) {
            p->actionLatch464 = 1;
            p->actionLatch465 = 1;
        }
        if (InMode(p, 6)) {
            p->sceneMode = 0;
            p->active = 0;
            p->secondaryDrawFlags &= ~0x400;
        }
        sub_08238580(p, source, target);
    }
    if (Test(0x10, &source->reactionFlags))
        LinkSession_ClearFirstPlayerKind(&p->position, p->slot);
    if (InMode(p, 3)) {
        target->cost = 0;
        amount = 0;
        if (Test(0x40000, &source->reactionFlags))
            LinkPlayer_FlushPendingEffects(p);
        goto done;
    }
    if ((InMode(p, 5)) || p->tetherMode == 2 || p->actionMode == 4)
        shift++;
    if (Test(4, &source->reactionFlags)) {
        if (!(target->costFlags & 2)) {
            if (p->contactSpecial == 0)
                p->contactActionTag = 0x191;
            sub_0813D548(p, 1);
        }
    } else if (Test(8, &source->reactionFlags)) {
        if (!(target->costFlags & 2)) {
            if (p->contactSpecial == 0)
                p->contactActionTag = 0x133;
            sub_0813D548(p, 2);
        }
    } else if (Test(0x40000, &source->reactionFlags))
        LinkPlayer_FlushPendingEffects(p);
    if (Test(0x8000, &source->reactionFlags))
        sub_0813D834(p, 0, 0x708);
    if (Test(0x100000, &source->reactionFlags))
        sub_0813D834(p, 1, 0x384);
    if (Test(0x20000, &source->reactionFlags)) {
        struct ContactPosition a, b;
        s32 angle = ((p->facing + 5) & 7) << 5;
        s32 v;
        v = gUnk_086149C4[(angle + 64) & 255] * 80;
        a.x = TruncShift(v, 12);
        a.y = 200;
        v = gUnk_086149C4[angle] * 80;
        a.z = TruncShift(v, 12);
        SceneParticle_CreateDefaultPriority((struct ContactPosition *)&p->position, &a, 0, 0, 1);
    }
    amount = target->cost;
    amount <<= shift;
    target->cost = 0;
    if (!(source->statRewardFlags & 0x10) && Test16(0xF, &source->contactFlags)) {
        if (Test(0x20, &source->reactionFlags))
            sub_08242FAC(Player_FindMaskedSlot((const struct PlayerSelection *)source), amount);
        if (Test(2, &source->reactionFlags))
            sub_08243020(Player_FindMaskedSlot((const struct PlayerSelection *)source), amount);
    }
    p->particleEmissionCount += amount;
done: {
    u16 f = source->contactFlags;
    *(u16 *)((u8 *)p + 0x54E) = f;
}
    *(u32 *)((u8 *)p + 0x550) = source->statRewardFlags;
    return amount;
}
