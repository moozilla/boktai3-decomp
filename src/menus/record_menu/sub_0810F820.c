// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* LinkLobbyPanel_ResetInstance: Clears the panel singleton pointer */
#define LinkLobbyPanel_ResetInstance sub_0810F820

void sub_0810F820(void)
{
    gActiveRecordMenu = 0;
}
