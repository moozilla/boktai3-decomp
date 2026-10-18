#include "global.h"
struct ColliderActorVector {
    s16 x, y, z, pad;
};
struct ColliderActor {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[2];
    struct ColliderActorVector position;
    u8 pad5C[0xa8 - 0x5c];
    u8 facing4, facing8;
    u8 padAA[0x32b - 0xaa];
    u8 direction;
    u8 pad32C[0x38a - 0x32c];
    u16 travelTime;
    u8 pad38C[0x39c - 0x38c];
    struct ColliderActorVector target;
};
u16 Sqrt(u32);
s32 __divsi3(s32, s32);
static inline void SetTravelTime(struct ColliderActor *actor, u32 time)
{
    actor->travelTime = time;
    if ((u16)time <= 1)
        actor->travelTime = 2;
}
s32 ColliderActor_SetTargetPosition(struct ColliderActor *actor, struct ColliderActorVector *target)
{
    struct ColliderActorVector delta;
    struct ColliderActorVector *out;
    struct ColliderActorVector *position;
    s32 distance, result;
    actor->target = *target;
    {
        u32 heading = actor->direction << 6;
        u32 flag = 128;
        u8 *dest = &actor->heading;
        *dest = heading | flag;
    }
    {
        s32 value = (actor->heading + 96) >> 6;
        u32 mask = 3;
        mask &= value;
        actor->facing4 = mask;
    }
    {
        s32 value = (actor->heading + 112) >> 5;
        u32 mask = 7;
        mask &= value;
        actor->facing8 = mask;
    }
    out = &delta;
    position = &actor->position;
    delta.x = target->x - position->x;
    out->y = target->y - position->y;
    out->z = target->z - position->z;
    distance = Sqrt(delta.x * delta.x + out->z * out->z);
    result = __divsi3(distance * 628, 25600);
    SetTravelTime(actor, result);
    /* All three direct ROM callers discard the incidental return register. */
}
