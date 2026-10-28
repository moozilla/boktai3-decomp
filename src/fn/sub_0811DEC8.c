#include "global.h"
#include "ram.h"
struct S {
    u32 a;
};
struct S *Modifier_GetTableTask(void)
{
    if (!(*(struct S **)&gScriptRecordTable))
        return 0;
    return &(*(struct S **)&gScriptRecordTable)->a;
}
