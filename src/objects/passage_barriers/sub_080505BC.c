#include "objects/passage_barriers/cell_gate.h"
void PassageBarrier_UpdateConditionalBarrier(struct CellGate *gate)
{
    s32 result = PassageBarrier_TestPlayerHeight(gate);
    if (result) {
        if (PassageBarrier_TestPlayerCell(gate)) {
            if (PassageBarrier_TestTraversalMode(gate))
                PassageBarrier_ClampPlayerEdge(gate, 64);
            else if (gate->registered) {
                u8 retained = gate->flags & 32;
                if (retained) {
                    u32 value = *(u16 *)(gate->registration + 6);
                    u32 mask = 64;
                    mask |= value;
                    *(u16 *)(gate->registration + 6) = mask;
                } else {
                    TerrainModifiers_UnregisterCell(gate->registration);
                    gate->registered = retained;
                    Player_BeginTerrainInteraction(gPlayers[0]);
                }
                if (gate->scriptId)
                    Script_Call(gate->scriptId, 0);
            }
        } else {
            if (PassageBarrier_TestTraversalMode(gate) && PassageBarrier_TestPlayerEdge(gate, 64)) {
                if (!gate->registered) {
                    TerrainModifiers_RegisterCell(gate->registration, gate->cell, 0, gate->height,
                                                  255, 0x82A);
                    gate->registered = 1;
                }
            } else if (gate->registered) {
                TerrainModifiers_UnregisterCell(gate->registration);
                gate->registered = 0;
            }
        }
    } else if (gate->registered) {
        TerrainModifiers_UnregisterCell(gate->registration);
        gate->registered = result;
    }
}
