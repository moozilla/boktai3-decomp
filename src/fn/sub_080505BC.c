#include "cell_gate.h"
void CellGate_UpdateConditionalBarrier(struct CellGate *gate)
{
    s32 result = CellGate_TestPlayerHeight(gate);
    if (result) {
        if (CellGate_TestPlayerCell(gate)) {
            if (CellGate_TestTraversalMode(gate))
                CellGate_ClampPlayerEdge(gate, 64);
            else if (gate->registered) {
                u8 retained = gate->flags & 32;
                if (retained) {
                    u32 value = *(u16 *)(gate->registration + 6);
                    u32 mask = 64;
                    mask |= value;
                    *(u16 *)(gate->registration + 6) = mask;
                } else {
                    sub_0821D6D0(gate->registration);
                    gate->registered = retained;
                    sub_08158CD8(gUnk_02000580);
                }
                if (gate->scriptId)
                    Script_Call(gate->scriptId, 0);
            }
        } else {
            if (CellGate_TestTraversalMode(gate) && CellGate_TestPlayerEdge(gate, 64)) {
                if (!gate->registered) {
                    sub_0821D698(gate->registration, gate->cell, 0, gate->height, 255, 0x82A);
                    gate->registered = 1;
                }
            } else if (gate->registered) {
                sub_0821D6D0(gate->registration);
                gate->registered = 0;
            }
        }
    } else if (gate->registered) {
        sub_0821D6D0(gate->registration);
        gate->registered = result;
    }
}
