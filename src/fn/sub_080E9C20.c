#include "global.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "scene_vec.h"

struct S080E9C20Anim {
    u8 unk0[8];
    u16 unk8;
    u8 unkA[4];
    u16 unkE;
};
struct S080E9C20Target {
    u8 unk0[8];
    u8 kind;
    u8 unk9[0x3C - 9];
    struct SceneActorTerrainCollisionView *unk3C;
};
struct Player;
s32 Player_BeginPositionRequest(struct Player *, struct SceneVec *, struct SceneVec *, s32, s32,
                                s32, s32, s32, u32);
void sub_0807D2E0(void *, u32, struct ContactPosition *, u32, u32, u32);
s32 SceneActorReaction_FindAdjacentTarget(struct SceneActorRegistryResetView *, s32);
void Octopus_UpdateHold(void);
void SceneActorReaction_PlayEntry17(struct SceneActorRegistryResetView *);
extern const struct ContactPosition gUnk_086062B0[];
extern void *gUnknown_02000580[];

static inline void S080E9C20_Facing(struct SceneActorRegistryResetView *actor, u8 *quadrant,
                                    u8 *flip)
{
    s32 side = ((actor->direction + 0x60) >> 6) & 3;
    if (side > 1) {
        *flip = 1;
        *quadrant = 3 - side;
    } else {
        *flip = 0;
        *quadrant = side;
    }
}

#ifdef NONMATCHING
// fuzzy: 41.4%
// scope shape 159 (differences once low registers are set aside)
// confidence=low: timed follow-up step of the reaction state machine; picks a throw offset from gUnk_086062B0
// and starts the player or actor position request, or rearms the PlayEntry17 state.
void sub_080E9C20(struct SceneActorRegistryResetView *actor)
{
    struct S080E9C20Anim *anim;
    struct S080E9C20Target *target;
    struct ContactPosition pos;
    const struct ContactPosition *e;
    u8 quadrant, flip;
    u32 started;
    s32 limit;
    u16 v8;

    anim = (struct S080E9C20Anim *)((u8 *)actor->drawable + 0x48);
    started = SceneActorReaction_TakePrepared(actor);
    if (started) {
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 5);
        if (actor->paletteOffset == 1)
            Anim_SetDurationScale(anim, 0x30);
    }
    v8 = anim->unk8;
    if (anim->unk8 == 3 && anim->unkE == 0)
        Sound_PlaySong(0x1DD);

    if (actor->paletteOffset == 1)
        limit = 3;
    else
        limit = 6;

    if (v8 == 3 && anim->unkE > limit) {
        actor->colliderA.flags = (actor->colliderA.flags | 4) & 0xFFF7;
        actor->flags2C0 |= 0x800;
        if (SceneActorReaction_FindAdjacentTarget(actor, 0)) {
            *((u8 *)actor + 0x2B2) = 1;
            *((u8 *)actor + 0x2B0) = 0;
            *((u8 *)actor + 0x2B5) = 0x25;
            actor->target = Octopus_UpdateHold;
            *(u16 *)((u8 *)actor + 0x2C8) = 0;
            actor->flags114 &= ~1;
            actor->word104 = 0;
            actor->state2A8 = 0;
            actor->phase2A9 = 2;
            actor->behavior2AA = 1;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
            SceneActorAnimation_SelectDirectionalFlags((void *)actor, 0x12);

            S080E9C20_Facing(actor, &quadrant, &flip);
            e = &gUnk_086062B0[flip * 5 + anim->unk8];
            pos.x = e->x + actor->position.x;
            pos.y = e->y + actor->position.y;
            pos.z = e->z + actor->position.z;
            target = *(struct S080E9C20Target **)((u8 *)actor + 0xDC);
            if (target->kind <= 3) {
                Player_BeginPositionRequest((void *)gUnknown_02000580[target->kind], &pos, 0, 0,
                                            flip, 0, 0, 0, 0);
            } else if (target->kind == 4) {
                struct SceneActorTerrainCollisionView *t = target->unk3C;
                *(u16 *)((u8 *)t + 0x178) = 0;
                SceneActorTerrain_ClearCollisionTimers(t);
                *(u16 *)((u8 *)t + 0x2D4) &= ~0x20;
                sub_0807D2E0(t, 0, &pos, 0, flip, 0);
            }
            return;
        }
    } else {
        if (v8 > 1) {
            actor->colliderA.flags = (actor->colliderA.flags | 4) & 0xFFF7;
            actor->flags2C0 |= 0x800;
        } else {
            actor->colliderA.flags = (actor->colliderA.flags & 0xFFFB) | 8;
            actor->flags2C0 &= 0xF7FF;
        }
    }

    if (actor->field2B3) {
        actor->colliderA.flags = (actor->colliderA.flags & 0xFFFB) | 8;
        actor->flags2C0 &= 0xF7FF;
        *((u8 *)actor + 0x2B2) = 1;
        *((u8 *)actor + 0x2B0) = 0;
        *((u8 *)actor + 0x2B5) = 0x27;
        actor->target = SceneActorReaction_PlayEntry17;
        *(u16 *)((u8 *)actor + 0x2C8) = 0;
        actor->flags114 &= ~1;
        actor->word104 = 0;
        actor->state2A8 = 0;
        actor->phase2A9 = 2;
        actor->behavior2AA = 3;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E9C20);
#endif
