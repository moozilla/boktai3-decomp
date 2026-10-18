#include "global.h"

extern u32 gUnk_02000208;
void Modifier_LoadTablesFromScript(u32);

s32 Modifier_InitTableTask(u32 a)
{
    Modifier_LoadTablesFromScript(a);
    gUnk_02000208 = a;
    return 1;
}
