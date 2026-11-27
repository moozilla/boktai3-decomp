// COMPILER: old_agbcc
#include "global.h"
#include "ui/dialogue/choice_menu.h"
/* TerrainModifiers_ResetState: Clears an auxiliary global (gChoiceMenu) associated with the terrain modifier subsystem; exact role unclear, best guess */

void TerrainModifiers_ResetState(void)
{
    gChoiceMenu = 0;
}
