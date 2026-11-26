#include "objects/passage_barriers/cell_gate.h"
s32 PassageBarrier_InitializeFromScript(struct CellGate *, u32, u32);
u32 PassageBarrier_Tick(struct CellGate *);
u32 PassageBarrier_Destroy(struct CellGate *);
struct CellGate *PassageBarrier_CreateFromScript(u32 id, u32 argument)
{
    struct CellGate *gate = Task_Create(8, 0x3C);
    if (gate) {
        Task_SetCallbacks(gate, (void (*)(void *))PassageBarrier_Tick,
                          (void (*)(void *))PassageBarrier_Destroy);
        if (PassageBarrier_InitializeFromScript(gate, id, argument) < 0) {
            Task_Kill(gate);
            return 0;
        }
    }
    return gate;
}
