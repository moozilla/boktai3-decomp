// COMPILER: old_agbcc
#include "global.h"
/* TerrainModifiers_ResetState: Clears an auxiliary global (gUnk_0200002C) associated with the terrain modifier subsystem; exact role unclear, best guess */
#define TerrainModifiers_ResetState sub_08006368

extern u32 gUnk_0200002C;

void sub_08006368(void)
{
    gUnk_0200002C = 0;
}
