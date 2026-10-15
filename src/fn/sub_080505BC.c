#include "cell_gate.h"
#define CellGate_UpdateConditionalBarrier sub_080505BC
void CellGate_UpdateConditionalBarrier(struct CellGate *gate)
{
    s32 result = sub_08050188(gate);
    if (result) {
        if (sub_08050128(gate)) {
            if (sub_08050574(gate))
                sub_080503A0(gate, 64);
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
            if (sub_08050574(gate) && sub_080501AC(gate, 64)) {
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
