// COMPILER: old_agbcc
#include "global.h"
#include "choice_menu.h"
/* TerrainModifiers_ResetState: Clears an auxiliary global (gChoiceMenu) associated with the terrain modifier subsystem; exact role unclear, best guess */
#define TerrainModifiers_ResetState sub_08006368

void sub_08006368(void)
{
    gChoiceMenu = 0;
}
