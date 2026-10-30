#include "scene_actor_reset.h"
#include "player.h"
#include "ram.h"

void SceneActorControl_OnIdle(void);
void Player_IncrementActionSpeedBonus(struct Player *player, u32 value);
void Player_QueueStatusEffect(struct Player *player, s32 effect);
void Sound_PlaySong(u32 song);
extern struct Player *gUnknown_02000580;

struct sub0809AE0C_Context {
    u8 unk0[0x3D4];
    u32 flags3D4;
};

static inline void sub0809AE0C_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                          void *target)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = target;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = 0;
}

static inline void sub0809AE0C_SetState(struct SceneActorRegistryResetView *actor, u32 state,
                                        u32 phase, u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

#ifdef NONMATCHING
// fuzzy: 40.7%
// scope shape 125 (differences once low registers are set aside)
// confidence=low: contact callback that starts a reaction when the other contact is flagged and the owner is near the player.
void sub_0809AE0C(void *unused, struct ContactCollider *other,
                  struct SceneActorRegistryResetView *actor)
{
    struct sub0809AE0C_Context *context;
    s32 limit;
    s32 distance;

    if (other->contactFlags != 1) {
        return;
    }
    {
        s32 reject;
        u32 m = 0x40;
        u32 n = 0x20000;
        reject = (actor != NULL) &&
                 ((actor->flags2D4 & m) || (actor->flags2D0 & n) || actor->health <= 0);
        if (reject) {
            return;
        }
    }
    distance = actor->body.position.y - gUnknown_02000580->position.y;
    {
        s32 fast = gPlayerPassageState != 0 && gPlayerInteractionMode == 5;
        if (fast || gUnknown_02000580->actionMode == 4) {
            limit = 0x3C;
        } else {
            limit = 0x104;
        }
    }
    distance = limit - distance;
    if (distance < 0) {
        distance = -distance;
    }
    if (distance > 0x20) {
        return;
    }
    sub0809AE0C_SetControl(actor, 16, SceneActorControl_OnIdle);
    sub0809AE0C_SetState(actor, 0, 2, 0);
    actor->colliderB.flags |= 4;
    actor->colliderA.flags |= 4;
    actor->flags2C0 |= 0x800;
    actor->flags2D6 &= ~0x180;
    if (actor->paletteOffset == 0) {
        context = actor->context;
        (void)(context->flags3D4 & 2);
        Player_IncrementActionSpeedBonus(gUnknown_02000580, 0);
        context = actor->context;
        context->flags3D4 |= 2;
        Sound_PlaySong(0x2AD);
        return;
    }
    context = actor->context;
    (void)(context->flags3D4 & 2);
    context = actor->context;
    context->flags3D4 |= 2;
    if (actor->paletteOffset == 1) {
        Player_QueueStatusEffect(gUnknown_02000580, 1);
    } else {
        Player_QueueStatusEffect(gUnknown_02000580, 2);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0809AE0C);
#endif
