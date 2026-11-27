// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* RecordPanel_ResetInstance: Clears the singleton task pointer for registry entry 3C9F */

void RecordPanel_ResetInstance(void)
{
    gConfirmPanel = 0;
}
