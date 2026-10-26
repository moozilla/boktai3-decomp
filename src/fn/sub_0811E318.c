#include "global.h"
#include "ram.h"

void Modifier_LoadTablesFromScript(u32);

s32 Modifier_InitTableTask(u32 a)
{
    Modifier_LoadTablesFromScript(a);
    gScriptRecordTable = a;
    return 1;
}
