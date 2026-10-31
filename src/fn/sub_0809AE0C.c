#include "scene_actor_reset.h"
#include "player.h"
#include "ram.h"

void SceneActorControl_OnIdle(void);
void Player_IncrementActionSpeedBonus(struct Player *player, u32 value);
void Player_QueueStatusEffect(struct Player *player, s32 effect);
void Sound_PlaySong(u32 song);
extern struct Player *gUnknown_02000580;

struct LunContactContext {
    u8 unknown0[0x3D4];
    u32 flags3D4;
};

static inline void Lun_SetControl(struct SceneActorRegistryResetView *actor, void *target, u32 kind)
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

static inline void Lun_SetState(struct SceneActorRegistryResetView *actor, u32 state, u8 phase,
                                u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

static inline u32 Lun_ReadContextFlags(struct SceneActorRegistryResetView *actor, u8 mask)
{
    struct LunContactContext *context = actor->context;
    return context->flags3D4 & mask ? 1 : 0;
}

static inline u32 Lun_ReadActorMotionFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D4 & mask;
}

static inline u32 Lun_ReadActorStatusFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2D0 & mask;
}

static inline u8 Lun_IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        if (Lun_ReadActorMotionFlags(actor, 0x40) || Lun_ReadActorStatusFlags(actor, 0x20000) ||
            actor->health <= 0)
            return 1;
    }
    return 0;
}

static inline u8 Lun_IsPlayerFast(void)
{
    u32 state = gPlayerPassageState;
    u32 result = 0;
    if (state && gPlayerInteractionMode == 5)
        result = 1;
    return result;
}

static inline s32 Lun_Abs(s32 value)
{
    if (value < 0)
        return -value;
    return value;
}

static inline void Lun_EnableCollider(struct ContactCollider *collider)
{
    collider->flags |= 4;
}

static inline void Lun_SetContactFlag(struct SceneActorRegistryResetView *actor, u16 bits)
{
    u16 *flags = &actor->flags2C0;
    *flags = bits | *flags;
}

static inline void Lun_ClearReactionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u16 *flags = &actor->flags2D6;
    u32 mask = ~bits;
    *flags &= mask;
}

static inline void Lun_SetContextFlags(struct LunContactContext *context, const u32 *mask)
{
    context->flags3D4 |= *mask;
}

static inline void Lun_ApplyContactFlag(struct LunContactContext *context)
{
    u32 mask = 2;
    Lun_SetContextFlags(context, &mask);
}

#define LUN_READ_CONTACT_FLAG(result_, actor_, mask_)                                              \
    do {                                                                                           \
        (result_) = Lun_ReadContextFlags(actor_, mask_);                                           \
    } while (0)

static inline u32 Lun_CapturePalette(struct SceneActorRegistryResetView *actor, u16 **entry)
{
    u16 *current = &actor->paletteOffset;
    u32 value = *current;
    *entry = current;
    return value;
}

/* Finishes the speed contact and returns from the collision callback. */
#define LUN_FINISH_SPEED_CONTACT(actor_, phase_)                                                   \
    do {                                                                                           \
        LUN_READ_CONTACT_FLAG((phase_), (actor_), (phase_));                                       \
        Player_IncrementActionSpeedBonus(gUnknown_02000580, 0);                                    \
        Lun_ApplyContactFlag((actor_)->context);                                                   \
        Sound_PlaySong(0x2AD);                                                                     \
        return;                                                                                    \
    } while (0)

/* Applies the palette-dependent player effect after a vertical contact check. */
void Lun_HandlePlayerContact(void *unused, struct ContactCollider *other, void *object)
{
    struct SceneActorRegistryResetView *actor;
    u8 phase;
    u16 *palette;
    s32 limit;
    s32 distance;

    if (other->contactFlags != 1) {
        return;
    }
    actor = object;
    if (Lun_IsUnavailable(actor))
        return;
    distance = actor->body.position.y - gUnknown_02000580->position.y;
    {
        s32 fast = Lun_IsPlayerFast();
        if (fast || gUnknown_02000580->actionMode == 4) {
            limit = 0x3C;
        } else {
            limit = 0x104;
        }
    }
    limit = Lun_Abs(limit - distance);
    if (limit > 0x20) {
        return;
    }
    Lun_SetControl(actor, SceneActorControl_OnIdle, 16);
    phase = 2;
    Lun_SetState(actor, 0, phase, 0);
    Lun_EnableCollider(&actor->colliderB);
    Lun_EnableCollider(&actor->colliderA);
    Lun_SetContactFlag(actor, 0x800);
    Lun_ClearReactionFlags(actor, 0x180);
    if (Lun_CapturePalette(actor, &palette) == 0) {
        LUN_FINISH_SPEED_CONTACT(actor, phase);
    }
    LUN_READ_CONTACT_FLAG(phase, actor, phase);
    Lun_ApplyContactFlag(actor->context);
    if (*palette == 1) {
        Player_QueueStatusEffect(gUnknown_02000580, 1);
    } else {
        Player_QueueStatusEffect(gUnknown_02000580, 2);
    }
}