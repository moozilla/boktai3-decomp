#include "global.h"
#include "contact_actor.h"
#include "engine.h"
u8 sub_0821D4F0(void *, void *, u32, u32);
struct MovementReachActor {
    u8 unknown0[0x51];
    u8 angle;
    u8 unknown52[2];
    struct ContactPosition position;
    u8 unknown5c[0x6c];
    u32 distanceSquared;
    u8 targetAngle;
    u8 unknowncd[0x43];
    struct ContactPosition *target;
};
static inline s32 cellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
s32 ActorMovement_CanReachTarget(struct MovementReachActor *p)
{
    s32 targetCell, actorCell;
    struct ContactPosition *position;
    u32 raw;
    s32 x, z;
    raw = (u16)p->target->x;
    x = (s16)raw >> 8;
    raw = (u16)p->target->z;
    z = (s16)raw >> 8;
    targetCell = cellIndex(x, z);
    {
        struct ContactPosition *view = &p->position;
        u32 raw;
        s32 x, z;
        raw = (u16)view->x;
        x = (s16)raw >> 8;
        raw = (u16)view->z;
        z = (s16)raw >> 8;
        position = view;
        actorCell = cellIndex(x, z);
    }
    if (targetCell == actorCell)
        return 1;
    if (((p->targetAngle - p->angle) & 255) + 0x30U > 0x60)
        return 0;
    if (p->distanceSquared > 0x90000)
        return 0;
    if (!sub_0821D4F0(position, p->target, 10, 256))
        return 0;
    return 1;
}
