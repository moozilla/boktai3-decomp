// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* LinkLobbyPanel_ResetInstance: Clears the panel singleton pointer */

void LinkLobbyPanel_ResetInstance(void)
{
    gActiveRecordMenu = 0;
}
