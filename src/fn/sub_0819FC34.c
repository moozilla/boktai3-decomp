#include "global.h"
#include "scene_actor_components.h"
#include "scene_facing.h"
#include "player.h"
#include "engine.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: charges along the facing axis, turning by a random signed offset on contact.
struct Action {
    u8 state, pad[3];
    u32 frame;
};
struct Hresvelgr {
    u8 unknown0[0x48];
    struct SceneActorBody body;
    u8 unknown90[12];
    u32 flags;
    u8 padA0[8];
    u8 direction4, direction8;
    u8 padAA[5];
    u8 finished, padB0, pending;
    u8 padB2[10];
    u32 contactFlags;
    u8 padC0[8];
    u32 targetDistance;
    u8 targetHeading;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[4];
    struct Player *player;
    struct ContactPosition *target;
    u8 pad114[0x60];
    u8 animationData[0x20], animation[0x20];
    u8 pad1B4[0x13c];
    struct Action action;
    u8 pad2F8[0x33];
    u8 side;
    u8 pad32C[8];
    s16 turn;
};
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void Actor_SetHeadingVelocity(void *, u32);
s32 Math_StepTowardTarget(s32, s32, s32);
static inline void SetHeading(struct Hresvelgr *p, s32 heading)
{
    p->body.direction = heading;
    p->direction4 = SceneFacing_FourWay(p->body.direction);
    p->direction8 = SceneFacing_EightWay(p->body.direction);
}
static inline u8 PlayerCanMove(struct Player *p, u32 mask)
{
    u8 result;
    u32 flags = p->statusFlags & mask;
    result = 0;
    if (!flags)
        result = 1;
    return result;
}
static inline u8 OutsideChargeBounds(struct Hresvelgr *actor)
{
    u8 result = 0;
    if (actor->body.position.x > 0x1400 || actor->body.position.z > 0x1400)
        result = 1;
    return result;
}
static inline s32 RandomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline void SetFlags(struct Hresvelgr *p, u32 mask)
{
    register u8 *address asm("r1");
    address = (u8 *)p;
    address += (u8 *)&p->flags - (u8 *)p;
    *(u32 *)address |= mask;
}
void Hresvelgr_UpdateTurningCharge(struct Hresvelgr *actor)
{
    struct Action *action = &actor->action;
    if (actor->pending) {
        actor->pending = 0;
        ActionState_Reset(action);
        actor->turn = 0;
        SetHeading(actor, actor->targetHeading);
        Sound_PlaySong(0x3db);
        Sound_PlaySong(0x4d3);
    }
    SetFlags(actor, 0x20008);
    if (!(u8)Callback_InvokeActor(actor, actor->callback)) {
        if (actor->contactFlags & 16)
            ActionState_SetState(action, 1);
        switch (action->state) {
            case 0:
                if (!action->frame) {
                    Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 2, 1, 0);
                    action->frame = 1;
                } else {
                    Actor_SetHeadingVelocity(actor, 32);
                    if (!actor->turn) {
                        u32 side = actor->side;
                        struct ContactPosition *target = actor->target;
                        u32 direction = ((actor->targetHeading + 96) >> 6) & 3;
                        if (!side) {
                            if (direction != 3)
                                actor->body.motion.z +=
                                    Math_StepTowardTarget(actor->body.position.z, target->z, 10) -
                                    actor->body.position.z;
                        } else if (direction != 0)
                            actor->body.motion.x +=
                                Math_StepTowardTarget(actor->body.position.x, target->x, 10) -
                                actor->body.position.x;
                    }
                    if (actor->targetDistance <= 0x3fff) {
                        if (PlayerCanMove(actor->player, 0x40000))
                            SetFlags(actor, 0x20000000);
                        actor->finished = 1;
                        ActionState_SetState(action, 2);
                    } else if (OutsideChargeBounds(actor))
                        ActionState_SetState(action, 2);
                }
                break;
            case 1:
                if (!action->frame) {
                    if (!actor->turn)
                        actor->turn = (((RandomValue() >> 3) % 2) << 5) - 16;
                    SetHeading(actor, actor->turn + actor->body.direction);
                    action->frame++;
                } else if (action->frame > 4)
                    ActionState_SetState(action, 0);
                else
                    action->frame++;
                break;
            case 2:
                actor->finished = 1;
                break;
        }
    }
}
