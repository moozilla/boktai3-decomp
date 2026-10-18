#include "cell_gate.h"
u32 CellGate_Destroy(struct CellGate *p)
{
    if (p->registered != 0)
        TerrainModifiers_UnregisterCell(p->registration);
}
