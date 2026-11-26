#include "objects/passage_barriers/cell_gate.h"
s32 PassageBarrier_TestPassage(struct CellGate *);
void PassageBarrier_UpdatePassageBarrier(struct CellGate *p)
{
    s32 r = PassageBarrier_TestPlayerHeight(p);
    if (r != 0) {
        if (PassageBarrier_TestPlayerCell(p) != 0 && (r = PassageBarrier_TestPassage(p)) == 0) {
            if (p->registered != 0) {
                TerrainModifiers_UnregisterCell(p->registration);
                p->registered = r;
                Player_BeginTerrainInteraction(gPlayers[0]);
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
