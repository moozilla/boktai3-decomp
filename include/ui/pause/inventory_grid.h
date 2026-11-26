#ifndef GUARD_INVENTORY_GRID_H
#define GUARD_INVENTORY_GRID_H

#include "scene/collision/contact_position.h"

struct SceneHudState;

void InventoryGrid_UpdateNumberPhase(struct SceneHudState *);
void InventoryGrid_BeginNumberPhase(struct SceneHudState *);
void InventoryGrid_BeginIconTransfer(struct SceneHudState *);
void InventoryGrid_ShowNumber(struct SceneHudState *, s32, const struct ContactPosition *);
void InventoryGrid_BeginNumberPhaseAlternate(struct SceneHudState *);
void InventoryGrid_FinishNumberPhase(struct SceneHudState *);

#endif
