#include "global.h"
struct T {
    u8 pad[0x1c];
    s32 count;
};
extern struct T *gUnk_02000208;

s32 sub_0811E3DC(void)
{
    if (!gUnk_02000208)
        return 0;
    return gUnk_02000208->count;
}
