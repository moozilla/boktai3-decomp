#include "cell_gate.h"
static inline u8 specialMode(void)
{
    u32 mode = gUnk_02000534;
    u32 result = 0;
    if (mode && gPlayerInteractionMode == 5)
        result = 1;
    return result;
}
void CellGate_UpdateForcedDirection(struct CellGate *gate)
{
    s32 result = CellGate_TestPlayerHeight(gate);
    if (result) {
        result = CellGate_TestPlayerCell(gate);
        if (result) {
            if (!gate->playerInside) {
                gUnk_02000580->gateCount++;
                if (gate->flags & 8)
                    gUnk_02000580->direction = 7;
                else
                    gUnk_02000580->direction = 1;
                gate->playerInside = 1;
            }
            if (gate->registered) {
                TerrainModifiers_UnregisterCell(gate->registration);
                gate->registered = 0;
            }
            CellGate_ClampPlayerEdge(gate, 40);
            if (gUnk_02000580->transitionMode != 4 && !specialMode()) {
                struct CellGatePlayer *player = gUnk_02000580;
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
                gUnk_02000580->gateCount--;
                gate->playerInside = result;
            }
            if (!CellGate_TestForcedPassage(gate)) {
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
            gUnk_02000580->gateCount--;
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
