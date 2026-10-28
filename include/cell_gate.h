#ifndef GUARD_CELL_GATE_H
#define GUARD_CELL_GATE_H
#include "contact_swarm.h"
#include "game_state.h"
#include "ram.h"
/* Tentative cell-local traversal gate; flags select behavior and constrained axis. */
struct CellGate {
    u8 unknown0[0x18];
    u16 id, unknown1A;
    u8 flags, registered;
    u16 cell, height, playerInside;
    u8 registration[0x10];
    u32 scriptId;
    void (*update)(struct CellGate *);
};
struct CellGatePlayer {
    u8 unknown0[0x1C];
    u32 flags;
    u8 unknown20[0x10];
    struct ContactPosition position;
    u8 unknown38[0x3E0];
    u8 actionMode;
    u8 unknown419[0x3E];
    u8 transitionMode;
    u8 unknown458[0x4D];
    u8 direction;
    u8 unknown4A6[0x3D];
    u8 gateCount;
};
typedef void (*CellGateUpdate)(struct CellGate *);
extern struct CellGatePlayer *gUnk_02000580;
s32 CellGate_TestPlayerHeight(struct CellGate *);
s32 CellGate_TestPlayerCell(struct CellGate *);
s32 CellGate_TestPlayerEdge(struct CellGate *, s32);
s32 CellGate_TestForcedPassage(struct CellGate *);
s32 CellGate_TestTraversalMode(struct CellGate *);
void CellGate_ClampPlayerEdge(struct CellGate *, s32);
void TerrainModifiers_UnregisterCell(void *);
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void Player_BeginTerrainInteraction(struct CellGatePlayer *);
void Player_BeginDirectionSequence(struct CellGatePlayer *, u32);
#endif
