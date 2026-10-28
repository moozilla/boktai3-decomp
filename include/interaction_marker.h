#ifndef GUARD_INTERACTION_MARKER_H
#define GUARD_INTERACTION_MARKER_H
#include "contact_swarm.h"
struct InteractionMarker {
    u8 task[0x18];
    u16 id;
    u8 nearby, bodyEnabled;
    u8 unknown1C[4], body[0x48];
    struct ContactSwarmDrawPacket primary;
    struct ContactPosition position;
    struct ContactSwarmDrawPacket secondary;
    u16 paletteBase, activationFrame, height, inputCooldown;
    u16 scriptArgs[2];
    u32 scriptId;
    u16 stateFrame, unknown112;
    void (*update)(struct InteractionMarker *);
};
struct InteractionMarkerPlayer {
    u8 unknown0[0x20];
    u32 flags;
    u8 unknown24[0xC];
    struct ContactPosition position;
    u8 unknown38[0x41F];
    u8 action;
    u8 unknown458[0xC];
    u8 busy;
};
extern struct InteractionMarkerPlayer *gUnk_02000580[4];
extern u32 gUnk_02000560;
extern u16 gPadState[];
u32 InteractionMarker_IsPlayerNearby(struct InteractionMarker *);
void InteractionMarker_SetState(struct InteractionMarker *, void (*)(struct InteractionMarker *));
void InteractionMarker_TickIdle(struct InteractionMarker *);
void InteractionMarker_TickInteraction(struct InteractionMarker *);
void InteractionMarker_TickDormant(struct InteractionMarker *);
void ScriptUtility_SetControlFlag(u32);
void PlayerCue_WalkTo(struct InteractionMarkerPlayer *, struct ContactPosition *, u32, u32);
u32 InteractionMarker_Tick(struct InteractionMarker *);
u32 InteractionMarker_Destroy(struct InteractionMarker *);
s32 InteractionMarker_InitializeFromScript(struct InteractionMarker *, u32, u32);
#endif
