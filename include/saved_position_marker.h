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
    u16 paletteBase, activationFrame, height, inputCooldown;
    u16 scriptArgs[2];
    u32 scriptId;
    u16 stateFrame, unknownC6;
    void (*update)(struct SavedPositionMarker *);
};
u32 SavedPositionMarker_IsStoredPositionNearby(struct SavedPositionMarker *);
void SavedPositionMarker_SetState(struct SavedPositionMarker *,
                                  void (*)(struct SavedPositionMarker *));
void SavedPositionMarker_TickIdle(struct SavedPositionMarker *);
void SavedPositionMarker_TickInteraction(struct SavedPositionMarker *);
void SavedPositionMarker_TickDormant(struct SavedPositionMarker *);
u32 SavedPositionMarker_Tick(struct SavedPositionMarker *);
u32 SavedPositionMarker_Destroy(struct SavedPositionMarker *);
s32 SavedPositionMarker_InitializeFromScript(struct SavedPositionMarker *, u32, u32);
#endif
