#include "global.h"
#include "engine.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: charges along the facing axis, turning by a random signed offset on contact.
struct Vec {
    s16 x, y, z, pad;
};
struct Action {
    u8 state, pad[3];
    u32 frame;
};
struct Player {
    u8 pad[0x20];
    u32 flags;
};
struct Hresvelgr {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[2];
    struct Vec position, velocity;
    u8 pad64[0x38];
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
    struct Vec *target;
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
    s32 shifted;
    register u32 mask asm("r1");
    p->heading = heading;
    shifted = (p->heading + 96) >> 6;
    mask = 3;
    mask &= shifted;
    asm("" : "+r"(mask));
    p->direction4 = mask;
    shifted = (p->heading + 112) >> 5;
    mask = 7;
    mask &= shifted;
    asm("" : "+r"(mask));
    p->direction8 = mask;
}
static inline u8 PlayerCanMove(struct Player *p, u32 mask)
{
    u8 result;
    u32 flags = p->flags & mask;
    result = 0;
    if (!flags)
        result = 1;
    return result;
}
static inline u8 OutsideChargeBounds(struct Hresvelgr *actor)
{
    u8 result = 0;
    if (actor->position.x > 0x1400 || actor->position.z > 0x1400)
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
static inline void SetFlags(struct Hresvelgr *actor, u32 bits)
{
    u32 mask = bits;
    register u8 *address asm("r1");
    address = (u8 *)actor;
    address += 0x9c;
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
                        struct Vec *target = actor->target;
                        u32 direction = ((actor->targetHeading + 96) >> 6) & 3;
                        if (!side) {
                            if (direction != 3)
                                actor->velocity.z +=
                                    Math_StepTowardTarget(actor->position.z, target->z, 10) -
                                    actor->position.z;
                        } else if (direction != 0)
                            actor->velocity.x +=
                                Math_StepTowardTarget(actor->position.x, target->x, 10) -
                                actor->position.x;
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
                    SetHeading(actor, actor->turn + actor->heading);
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
