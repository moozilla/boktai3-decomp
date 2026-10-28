#include "global.h"

/* Partial actor view used by the held-key motion controls. */
struct MotionControlActor {
    u8 unk_00[0x38];
    s32 restrictionMetric;
    u8 unk_3C[0x1C];
    s16 health;
    u8 unk_5A[0x66];
    s32 boost;
    u8 unk_C4[0x4E8];
    s32 forwardSpeed;
    s32 verticalSpeed;
    s32 lateralSpeed;
    u8 unk_5B8[0x24];
    s32 movementSpeed;
    u8 unk_5E0[0x38];
    /* Opaque embedded sprite source and its unmodeled trailing state. */
    u8 boostSource[0x292];
    s8 boostEffect;
    u8 unk_8AB[0xD];
    u16 boostFrames;
};

extern u16 gPadState;
s8 SpriteSlots_AttachSource(void *);
void TimedSpriteEffect_RequestStop(u32);
s32 Div(s32, s32);

static inline void SetHealth(struct MotionControlActor *actor, s32 health)
{
    if (health < 0)
        health = 0;
    actor->health = health;
}

/* Starting a boost costs twenty health; continued use costs two per frame. */
static inline void ApplyBoost(struct MotionControlActor *actor)
{
    struct MotionControlActor *owner = actor;
    if (actor->health > 0 && (gPadState & 0x200)) {
        s32 health;
        actor->boost += 0x1000;
        if (actor->boost > 0x3000)
            actor->boost = 0x3000;
        if (owner->boostFrames == 0) {
            health = actor->health - 20;
            if (owner->boostEffect < 0)
                owner->boostEffect = SpriteSlots_AttachSource(actor->boostSource);
        } else {
            health = actor->health - 2;
        }
        SetHealth(actor, health);
        owner->boostFrames++;
    } else {
        if (owner->boostEffect >= 0) {
            TimedSpriteEffect_RequestStop(owner->boostEffect);
            owner->boostEffect = -1;
        }
        owner->boostFrames = 0;
    }
}

void ActorMotion_ApplyInput(struct MotionControlActor *actor)
{
    ApplyBoost(actor);
    if (gPadState & 0x100) {
        actor->forwardSpeed += 0x200;
        if (actor->forwardSpeed > 0x5000)
            actor->forwardSpeed = 0x5000;
    }
    if (gPadState & 0x40)
        actor->lateralSpeed -= 0x200;
    else if (gPadState & 0x80)
        actor->lateralSpeed += 0x200;
    if (actor->verticalSpeed >= 0 && (actor->restrictionMetric >> 12) == 0) {
        if (gPadState & 0x10) {
            actor->movementSpeed += 0x1000;
            if (actor->movementSpeed > 0xC8000)
                actor->movementSpeed = 0xC8000;
        } else if (gPadState & 0x20) {
            actor->movementSpeed -= 0x1000;
            if (actor->movementSpeed < 0x20000)
                actor->movementSpeed = 0x20000;
        }
    }
    actor->forwardSpeed -= 0x20;
    actor->boost -= 0x80;
    if (actor->forwardSpeed < 0x3000)
        actor->forwardSpeed = 0x3000;
    if (actor->boost < 0)
        actor->boost = 0;
    actor->lateralSpeed = Div(actor->lateralSpeed * 90, 100);
}
