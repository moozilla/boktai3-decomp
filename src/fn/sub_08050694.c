#include "cell_gate.h"
u32 CellGate_Tick(struct CellGate *p)
{
    if (!(gPlayers[0]->kind & 4))
        Callback_InvokeActor(p, p->update);
    return 0;
}
