#include "global.h"
struct ActorSavedPoint {
    u32 coordinates[2];
};
struct ActorPositionState {
    u8 unknown0[0x110];
    struct ActorSavedPoint *selectedPoint;
    u8 unknown114[0x364];
    struct ActorSavedPoint savedPoint;
};
s32 ActorMovement_CanReachTarget(struct ActorPositionState *);
s32 ActorPosition_SaveSelectedPoint(struct ActorPositionState *p)
{
    s32 result;
    if (!ActorMovement_CanReachTarget(p))
        result = 0;
    else {
        struct ActorSavedPoint *source = p->selectedPoint;
        p->savedPoint = *source;
        result = 1;
    }
    return result;
}
