#include "global.h"
#include "ram.h"

s32 sub_0811E5B0(void)
{
    s32 count;
    if (gScriptRecordTable)
        count = *(s32 *)(gScriptRecordTable + 0x1a8);
    else
        count = 0;
    return count;
}
