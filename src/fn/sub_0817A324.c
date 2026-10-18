#include "global.h"
#define ActorPosition_SaveSelectedPoint sub_0817A324
struct ActorSavedPoint {
    u32 coordinates[2];
};
struct ActorPositionState {
    u8 unknown0[0x110];
    struct ActorSavedPoint *selectedPoint;
    u8 unknown114[0x364];
    struct ActorSavedPoint savedPoint;
};
s32 sub_0817A23C(struct ActorPositionState *);
s32 sub_0817A324(struct ActorPositionState *p)
{
    s32 result;
    if (!sub_0817A23C(p))
        result = 0;
    else {
        struct ActorSavedPoint *source = p->selectedPoint;
        p->savedPoint = *source;
        result = 1;
    }
    return result;
}
