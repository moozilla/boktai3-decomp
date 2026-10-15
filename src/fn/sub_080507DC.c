#include "cell_gate.h"
#define CellGate_CreateFromScript sub_080507DC
s32 sub_080506CC(struct CellGate *, u32, u32);
u32 sub_08050694(struct CellGate *);
u32 sub_080506B8(struct CellGate *);
struct CellGate *CellGate_CreateFromScript(u32 id, u32 argument)
{
    struct CellGate *gate = Task_Create(8, 0x3C);
    if (gate) {
        Task_SetCallbacks(gate, (void (*)(void *))sub_08050694, (void (*)(void *))sub_080506B8);
        if (sub_080506CC(gate, id, argument) < 0) {
            Task_Kill(gate);
            return 0;
        }
    }
    return gate;
}
