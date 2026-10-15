#include "cell_gate.h"
#define CellGate_Destroy sub_080506B8
u32 sub_080506B8(struct CellGate *p)
{
    if (p->registered != 0)
        sub_0821D6D0(p->registration);
}
