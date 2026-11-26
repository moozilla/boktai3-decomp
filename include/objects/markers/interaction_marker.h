#ifndef GUARD_INTERACTION_MARKER_H
#define GUARD_INTERACTION_MARKER_H
#include "scene/collision/contact_swarm.h"
#include "engine/input.h"
#include "engine/task.h"
#include "player/player.h"
struct InteractionMarker {
    struct Task task;
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
extern u32 gUnk_02000560;
u32 InteractionMarker_IsPlayerNearby(struct InteractionMarker *);
void InteractionMarker_SetState(struct InteractionMarker *, void (*)(struct InteractionMarker *));
void InteractionMarker_TickIdle(struct InteractionMarker *);
void InteractionMarker_TickInteraction(struct InteractionMarker *);
void InteractionMarker_TickDormant(struct InteractionMarker *);
void ScriptUtility_SetControlFlag(u32);
void PlayerCue_WalkTo(struct Player *, struct ContactPosition *, u32, u32);
u32 InteractionMarker_Tick(struct InteractionMarker *);
u32 InteractionMarker_Destroy(struct InteractionMarker *);
s32 InteractionMarker_InitializeFromScript(struct InteractionMarker *, u32, u32);
#endif
