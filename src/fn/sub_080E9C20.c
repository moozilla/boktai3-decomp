#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"
#include "display_sequence_inline.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_terrain.h"

struct OctopusCaptureTarget {
    u8 unk0[8];
    u8 kind;
    u8 unk9[0x3C - 9];
    struct SceneActorRegistryResetView *actor;
};
struct Player;
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void sub_0807D2E0(void *, u32, struct ContactPosition *, u32, u32, u32);
s32 SceneActorReaction_FindAdjacentTarget(struct SceneActorRegistryResetView *, s32);
void Octopus_UpdateHold(void);
void SceneActorReaction_PlayEntry17(struct SceneActorRegistryResetView *);
struct OctopusOffset {
    u16 x, y, z, pad;
};
extern const struct OctopusOffset gUnk_086062B0[2][5];

static inline void Octopus_GetCaptureFacing(struct SceneActorRegistryResetView *actor, u8 *quadrant,
                                            u8 *flip)
{
    s16 side = ((actor->body.direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

static inline void Octopus_EnableCollider(struct ContactCollider *collider)
{
    collider->flags |= 4;
    collider->flags &= 0xFFF7;
}
static inline void Octopus_SetContactFlag(struct SceneActorRegistryResetView *actor, u32 bits)
{
    actor->flags2C0 = bits | actor->flags2C0;
}
static inline void Octopus_DisableContact(struct SceneActorRegistryResetView *actor)
{
    actor->colliderA.flags &= 0xFFFB;
    actor->colliderA.flags |= 8;
    SceneActorChase_ClearFlagComponent((void *)&actor->flags2C0, 0x800);
}

struct OctopusFrameIndex {
    u16 index;
};
static inline struct OctopusFrameIndex Octopus_ReadFrame(struct DisplaySequenceState *sequence)
{
    struct OctopusFrameIndex value = {0};
    value.index = sequence->index;
    return value;
}

void sub_080E9C20(struct SceneActorRegistryResetView *actor)
{
    struct DisplaySequenceState *anim;
    struct OctopusCaptureTarget *target;
    struct ContactPosition pos;
    u8 quadrant, flip;
    u32 started;
    s32 limit;
    u16 *palette;

    anim = &((struct SceneActorControlMotionDrawable *)actor->drawable)->sequence;
    started = SceneActorReaction_TakePrepared(actor);
    palette = &actor->paletteOffset;
    if (started) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        if (*palette == 1)
            Anim_SetDurationScale(anim, 0x30);
    }
    if (anim->index == 3 && anim->elapsed == 0)
        Sound_PlaySong(0x1DD);

    if (*palette == 1)
        limit = 3;
    else
        limit = 6;

    if (Octopus_ReadFrame(anim).index == 3 && anim->elapsed > limit) {
        Octopus_EnableCollider(&actor->colliderA);
        Octopus_SetContactFlag(actor, 0x800);
        if (SceneActorReaction_FindAdjacentTarget(actor, 0)) {
            SceneActorChase_SetControl(actor, Octopus_UpdateHold, 0x25);
            SceneActorChase_SetState(actor, 0, 2, 1);
            SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x12);

            Octopus_GetCaptureFacing(actor, &quadrant, &flip);
            pos.x = actor->body.position.x + gUnk_086062B0[flip][anim->index].x;
            pos.y = actor->body.position.y + gUnk_086062B0[flip][anim->index].y;
            pos.z = actor->body.position.z + gUnk_086062B0[flip][anim->index].z;
            target = (void *)actor->tracking.target;
            if (target->kind <= 3) {
                Player_BeginPositionRequest((void *)gUnknown_02000580[target->kind], (void *)&pos,
                                            0, 0, flip, 0, 0, 0, 0);
            } else if (target->kind == 4) {
                struct SceneActorRegistryResetView *t = target->actor;
                t->colliderA.responseValue = 0;
                SceneActorTerrain_ClearCollisionTimers((void *)t);
                SceneActorChase_ClearMotionFlagComponent(t, 0x20);
                sub_0807D2E0(t, 0, (void *)&pos, 0, flip, 0);
            }
            return;
        }
    } else if (Octopus_ReadFrame(anim).index <= 1) {
        Octopus_DisableContact(actor);
    } else {
        actor->colliderA.flags |= 4;
        actor->colliderA.flags &= 0xFFF7;
        Octopus_SetContactFlag(actor, 0x800);
    }

    if (actor->field2B3) {
        Octopus_DisableContact(actor);
        SceneActorChase_SetControl(actor, SceneActorReaction_PlayEntry17, 0x27);
        SceneActorChase_SetState(actor, 0, 2, 3);
    }
}
