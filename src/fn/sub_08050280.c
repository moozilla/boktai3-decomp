#include "cell_gate.h"
s32 CellGate_TestPassage(struct CellGate *);
void CellGate_UpdatePassageBarrier(struct CellGate *p)
{
    s32 r = CellGate_TestPlayerHeight(p);
    if (r != 0) {
        if (CellGate_TestPlayerCell(p) != 0 && (r = CellGate_TestPassage(p)) == 0) {
            if (p->registered != 0) {
                TerrainModifiers_UnregisterCell(p->registration);
                p->registered = r;
                Player_BeginTerrainInteraction(gPlayers);
                if (p->scriptId != 0)
                    Script_Call(p->scriptId, 0);
            }
        } else if (p->registered == 0) {
            TerrainModifiers_RegisterCell(p->registration, p->cell, 0, p->height, 0xff, 0x82a);
            p->registered = 1;
        }
    } else if (p->registered != 0) {
        TerrainModifiers_UnregisterCell(p->registration);
        p->registered = r;
    }
}
