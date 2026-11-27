#include "objects/passage_barriers/cell_gate.h"
u32 PassageBarrier_Destroy(struct CellGate *p)
{
    if (p->registered != 0)
        TerrainModifiers_UnregisterCell(p->registration);
}
