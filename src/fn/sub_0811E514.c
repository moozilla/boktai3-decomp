#include "global.h"
#include "ram.h"

s32 sub_0811E514(void)
{
    s32 count;
    if (gScriptRecordTable)
        count = *(s32 *)(gScriptRecordTable + 0x13c);
    else
        count = 0;
    return count;
}
