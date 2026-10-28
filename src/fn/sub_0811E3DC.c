#include "global.h"
#include "ram.h"
struct T {
    u8 pad[0x1c];
    s32 count;
};

s32 sub_0811E3DC(void)
{
    if (!((struct T *)gScriptRecordTable))
        return 0;
    return ((struct T *)gScriptRecordTable)->count;
}
