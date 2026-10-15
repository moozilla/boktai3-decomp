#include "cell_gate.h"
s32 CellGate_InitializeFromScript(struct CellGate *, u32, u32);
u32 CellGate_Tick(struct CellGate *);
u32 CellGate_Destroy(struct CellGate *);
struct CellGate *CellGate_CreateFromScript(u32 id, u32 argument)
{
    struct CellGate *gate = Task_Create(8, 0x3C);
    if (gate) {
        Task_SetCallbacks(gate, (void (*)(void *))CellGate_Tick,
                          (void (*)(void *))CellGate_Destroy);
        if (CellGate_InitializeFromScript(gate, id, argument) < 0) {
            Task_Kill(gate);
            return 0;
        }
    }
    return gate;
}
