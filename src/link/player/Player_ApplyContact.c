#include "global.h"
#include "engine/engine.h"
#include "player/player.h"
#include "scene/collision/contact_actor.h"
#include "player/player_slot_status.h"
#include "engine/math/scene_vec_polar.h"

struct PlayerSelection;

void Collision_CalculateElementalResponse(struct ContactCollider *, struct ContactCollider *);
void Player_ApplyLinkedContactResponse(struct Player *, struct ContactCollider *,
                                       struct ContactCollider *);
s32 LinkSession_ClearFirstPlayerKind(void *, s32);
void LinkPlayer_FlushPendingEffects(struct Player *);
void Player_QueueStatusEffect(struct Player *, s32);
void sub_0813D834(struct Player *, s32, s32);
s32 SceneParticle_CreateDefaultPriority(const struct ContactPosition *,
                                        const struct ContactPosition *,
                                        const struct ContactPosition *,
                                        const struct ContactPosition *, s32);
struct Player *Player_FindMaskedSlot(const struct PlayerSelection *);
void sub_08242FAC(struct Player *, u32);
void sub_08243020(struct Player *, u32);

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

u32 Player_ApplyContact(struct Player *p, struct ContactCollider *source,
                        struct ContactCollider *target)
{
    u32 amount;
    u32 shift;

    {
        s32 count = source->unknown3C;
        if (count > 0)
            Collision_CalculateElementalResponse(source, target);
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
        Player_ApplyLinkedContactResponse(p, source, target);
    }
    if (Player_ReadStatusBits(&source->reactionFlags, 0x10))
        LinkSession_ClearFirstPlayerKind(&p->position, p->slot);
    if (InMode(p, 3)) {
        target->cost = 0;
        amount = 0;
        if (Player_ReadStatusBits(&source->reactionFlags, 0x40000))
            LinkPlayer_FlushPendingEffects(p);
        goto done;
    }
    if ((InMode(p, 5)) || p->tetherMode == 2 || p->actionMode == 4)
        shift++;
    if (Player_ReadStatusBits(&source->reactionFlags, 4)) {
        if (!(target->costFlags & 2)) {
            if (p->contactSpecial == 0)
                p->contactActionTag = 0x191;
            Player_QueueStatusEffect(p, 1);
        }
    } else if (source->reactionFlags & 8) {
        if (!(target->costFlags & 2)) {
            if (p->contactSpecial == 0)
                p->contactActionTag = 0x133;
            Player_QueueStatusEffect(p, 2);
        }
    } else if (source->reactionFlags & 0x40000)
        LinkPlayer_FlushPendingEffects(p);
    if (Player_ReadStatusBits(&source->reactionFlags, 0x8000))
        sub_0813D834(p, 0, 0x708);
    if (Player_ReadStatusBits(&source->reactionFlags, 0x100000))
        sub_0813D834(p, 1, 0x384);
    if (Player_ReadStatusBits(&source->reactionFlags, 0x20000)) {
        struct ContactPosition a, b;
        s32 angle = ((p->facing + 5) & 7) << 5;
        s32 v;
        v = gUnk_086149C4[(angle + 64) & 255] * 80;
        a.x = SceneVec_TruncateComponent(v);
        a.y = 200;
        v = gUnk_086149C4[angle] * 80;
        a.z = SceneVec_TruncateComponent(v);
        SceneParticle_CreateDefaultPriority((struct ContactPosition *)&p->position, &a, 0, 0, 1);
    }
    amount = target->cost;
    amount <<= shift;
    target->cost = 0;
    if (!(source->statRewardFlags & 0x10) && ContactCollider_TestContactFlags(source, 0xF)) {
        if (Player_ReadStatusBits(&source->reactionFlags, 0x20))
            sub_08242FAC(Player_FindMaskedSlot((const struct PlayerSelection *)source), amount);
        if (Player_ReadStatusBits(&source->reactionFlags, 2))
            sub_08243020(Player_FindMaskedSlot((const struct PlayerSelection *)source), amount);
    }
    p->particleEmissionCount += amount;
done: {
    u16 f = source->contactFlags;
    p->lastContactFlags = f;
}
    p->lastContactRewardFlags = source->statRewardFlags;
    return amount;
}
