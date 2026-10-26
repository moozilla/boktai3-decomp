#include "global.h"
#include "ram.h"
u32 Modifier_OnTableTaskDestroy(void)
{
    gScriptRecordTable = 0;
}
