#ifndef GUARD_SAVED_POSITION_MARKER_H
#define GUARD_SAVED_POSITION_MARKER_H
#include "interaction_marker.h"
struct SavedPositionMarker {
    u8 task[0x18];
    u16 nearby;
    u8 variant, unknown1B;
    struct ContactSwarmDrawPacket primary;
    struct ContactPosition position;
    struct ContactSwarmDrawPacket secondary;
    u16 baseTile, activationFrame, height, inputCooldown;
    u16 scriptArgs[2];
    u32 scriptId;
    u16 stateFrame, unknownC6;
    void (*update)(struct SavedPositionMarker *);
};
u32 sub_08053D8C(struct SavedPositionMarker *);
void sub_08053DE8(struct SavedPositionMarker *, void (*)(struct SavedPositionMarker *));
void sub_08053DF8(struct SavedPositionMarker *);
void sub_08053F48(struct SavedPositionMarker *);
void sub_08053FF4(struct SavedPositionMarker *);
u32 sub_08053FF8(struct SavedPositionMarker *);
u32 sub_0805400C(struct SavedPositionMarker *);
s32 sub_08054028(struct SavedPositionMarker *, u32, u32);
#endif
