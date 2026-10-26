#include "global.h"
#include "ram.h"

s32 sub_0811E470(void)
{
    s32 count;
    if (gScriptRecordTable)
        count = *(s32 *)(gScriptRecordTable + 0xe8);
    else
        count = 0;
    return count;
}
