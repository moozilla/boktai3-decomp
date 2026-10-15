#include "cell_gate.h"
#define CellGate_Tick sub_08050694
u32 sub_08050694(struct CellGate *p)
{
    if (!(gUnk_02000580->flags & 4))
        sub_0824923C(p, p->update);
    return 0;
}
