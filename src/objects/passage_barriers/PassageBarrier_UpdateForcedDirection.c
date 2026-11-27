#include "objects/passage_barriers/cell_gate.h"
static inline u8 specialMode(void)
{
    u32 mode = gPlayerPassageState;
    u32 result = 0;
    if (mode && gPlayerInteractionMode == 5)
        result = 1;
    return result;
}
void PassageBarrier_UpdateForcedDirection(struct CellGate *gate)
{
    s32 result = PassageBarrier_TestPlayerHeight(gate);
    if (result) {
        result = PassageBarrier_TestPlayerCell(gate);
        if (result) {
            if (!gate->playerInside) {
                gPlayers[0]->gateCount++;
                if (gate->flags & 8)
                    gPlayers[0]->coordinateAxis = 7;
                else
                    gPlayers[0]->coordinateAxis = 1;
                gate->playerInside = 1;
            }
            if (gate->registered) {
                TerrainModifiers_UnregisterCell(gate->registration);
                gate->registered = 0;
            }
            PassageBarrier_ClampPlayerEdge(gate, 40);
            if (gPlayers[0]->tetherMode != 4 && !specialMode()) {
                struct Player *player = gPlayers[0];
                u32 action = player->actionMode;
                if (action == 3)
                    goto done;
                if (action == 4)
                    goto done;
                {
                    if (gate->flags & 8)
                        Player_BeginDirectionSequence(player, 7);
                    else
                        Player_BeginDirectionSequence(player, 1);
                }
            }
        } else {
            if (gate->playerInside) {
                gPlayers[0]->gateCount--;
                gate->playerInside = result;
            }
            if (!PassageBarrier_TestForcedPassage(gate)) {
                if (!gate->registered) {
                    TerrainModifiers_RegisterCell(gate->registration, gate->cell, 0, gate->height,
                                                  255, 0x80B);
                    gate->registered = 1;
                }
            } else
                goto remove;
        }
    } else {
        if (gate->playerInside) {
            gPlayers[0]->gateCount--;
            gate->playerInside = result;
        }
    remove:
        if (gate->registered) {
            TerrainModifiers_UnregisterCell(gate->registration);
            gate->registered = result;
        }
    }
done:
    return;
}
