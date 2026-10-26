#include "global.h"
struct S {
    u32 a;
};
extern struct S *gScriptRecordTable;
struct S *Modifier_GetTableTask(void)
{
    if (!gScriptRecordTable)
        return 0;
    return &gScriptRecordTable->a;
}
