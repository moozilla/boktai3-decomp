#ifndef GUARD_CELL_GATE_H
#define GUARD_CELL_GATE_H
#include "engine/shared/contact_swarm.h"
#include "system/game_state/game_state.h"
#include "engine/shared/ram.h"
#include "engine/shared/task.h"
#include "player/player.h"
/* Tentative cell-local traversal gate; flags select behavior and constrained axis. */
struct CellGate {
    struct Task task;
    u16 id, unknown1A;
    u8 flags, registered;
    u16 cell, height, playerInside;
    u8 registration[0x10];
    u32 scriptId;
    void (*update)(struct CellGate *);
};
typedef void (*CellGateUpdate)(struct CellGate *);
s32 CellGate_TestPlayerHeight(struct CellGate *);
s32 CellGate_TestPlayerCell(struct CellGate *);
s32 CellGate_TestPlayerEdge(struct CellGate *, s32);
s32 CellGate_TestForcedPassage(struct CellGate *);
s32 CellGate_TestTraversalMode(struct CellGate *);
void CellGate_ClampPlayerEdge(struct CellGate *, s32);
void TerrainModifiers_UnregisterCell(void *);
void TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);
void Player_BeginTerrainInteraction(struct Player *);
void Player_BeginDirectionSequence(struct Player *, u32);
#endif
