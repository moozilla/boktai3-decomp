#include "cell_gate.h"
u32 CellGate_Tick(struct CellGate *p)
{
    if (!(gUnk_02000580->flags & 4))
        Callback_InvokeActor(p, p->update);
    return 0;
}
