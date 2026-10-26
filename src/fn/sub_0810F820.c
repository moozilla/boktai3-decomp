// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* LinkLobbyPanel_ResetInstance: Clears the panel singleton pointer */
#define LinkLobbyPanel_ResetInstance sub_0810F820

void sub_0810F820(void)
{
    gActiveRecordMenu = 0;
}
