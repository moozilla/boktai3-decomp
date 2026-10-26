#include "global.h"
struct T {
    u8 pad[0x1c];
    s32 count;
};
extern struct T *gScriptRecordTable;

s32 sub_0811E3DC(void)
{
    if (!gScriptRecordTable)
        return 0;
    return gScriptRecordTable->count;
}
