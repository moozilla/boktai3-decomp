#include "global.h"

// confidence=low: release animation for a captured target; on the release frame, throws a
// captured actor (kind & 4) or player (kind <= 3) from the arm's offset.
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "gba/macro.h"
#include "gba/syscall.h"

struct Player;
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void SceneActorReaction_ApplyHitResponse(void *, u32, struct ContactPosition *, u32, u32, u32);

struct OctopusTarget {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[0x33];
    void *player;
};

struct OctopusDrawable {
    u8 unknown0[7];
    u8 mode : 8;
};

struct OctopusOffset {
    u16 x, y, z, pad;
};

static inline void Facing(struct SceneActorRegistryResetView *actor, u8 *quadrant, u8 *flip)
{
    s32 side = ((actor->body.direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

void SpriteParticle_SpawnBatch(void *, u32, u32, u32, u32);
static inline void Hide(struct SceneActorRegistryResetView *a)
{
    u32 bit = 1;
    if (a->drawableKind == 0)
        *(u32 *)a->drawable |= bit;
    else {
        u8 *p = (u8 *)a->drawable + 0x20;
        *(u32 *)(p + 8) |= bit;
    }
}

extern const struct OctopusOffset gUnk_08606300[2][2];
void *SceneActorRegistry_FindById(u32);
void Octopus_ThrowCapturedTarget(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *sequence = (void *)((u8 *)actor->drawable + 0x48);
    struct OctopusTarget *target;
    s32 index;
    u8 quadrant, flip;
    struct ContactPosition position;
    void *who;
    void *player;
    if (SceneActorReaction_TakePrepared(actor)) {
        SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x13);
        {
            u16 *d = &actor->colliderA.flags;
            u32 flags = *d;
            u32 mask = 4;
            mask |= flags;
            mask &= 0xFFF7;
            *d = mask;
        }
        {
            u32 bit = 0x800;
            actor->flags2C0 = bit | actor->flags2C0;
        }
    }
    target = (void *)actor->tracking.target;
    if (target->kind <= 3 && sequence->elapsed + 1 >= sequence->duration) {
        index = sequence->index + 1;
        if (index >= sequence->frameCount)
            index = 0;
    } else
        index = sequence->index;
    Facing(actor, &quadrant, &flip);
    position.x = actor->body.position.x + gUnk_08606300[flip][index].x;
    position.y = actor->body.position.y + gUnk_08606300[flip][index].y;
    position.z = actor->body.position.z + gUnk_08606300[flip][index].z;
    if (target->kind & 4) {
        who = target->player;
        SceneActorReaction_ApplyHitResponse(who, 1, &position, 0, flip, 0);
    } else if (target->kind <= 3) {
        player = target->player;
        Player_BeginPositionRequest(player, (void *)&position, 0, 3, flip, 0, 0, 0, 0);
    }
    if (actor->field2B3) {
        actor->field2B0 = 1;
        ((struct OctopusDrawable *)actor->drawable)->mode = 2;
        Hide(actor);
        Sound_PlaySong(0x1E0);
        if (target->kind & 4) {
            who = SceneActorRegistry_FindById(*(u32 *)target);
            SceneActorReaction_ApplyHitResponse(who, 2, &position, 0, 0, 0);
            SpriteParticle_SpawnBatch(&actor->body.position, 1, 2, 0, 0x154);
        } else if (target->kind <= 3) {
            player = target->player;
            Player_BeginPositionRequest(player, (void *)&position, 0, 3, flip, 0, 1, 0, 0);
            SpriteParticle_SpawnBatch(&actor->body.position, 1, 1, 0, 0x154);
        }
    }
}
