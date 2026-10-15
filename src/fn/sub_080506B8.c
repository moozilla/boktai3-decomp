#include "cell_gate.h"
u32 CellGate_Destroy(struct CellGate *p)
{
    if (p->registered != 0)
        sub_0821D6D0(p->registration);
}
