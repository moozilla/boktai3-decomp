#ifndef GUARD_ACTOR_SPATIAL_QUERY_H
#define GUARD_ACTOR_SPATIAL_QUERY_H

#include "contact_position.h"

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

struct SceneVolumeSearch {
    u8 pad0[0x48];
    u32 searchFlags;
    u8 pad4C[8];
    s16 x, y, z;
    u8 pad5A[0x426];
    u16 coordinates[3];
};

s32 ActorMovement_CanReachTarget(struct MovementReachActor *);
s32 SceneVolume_FindNearbyPosition(struct SceneVolumeSearch *);

#endif
