#include "global.h"
extern u8 *gUnk_030052F4;
struct Map {
    u8 pad[0x1c];
    s16 offsets[4];
};
s32 sub_0821BCE4(s32 start, s32 target)
{
    s32 i, j;
    for (i = 0; i < 4; i++) {
        s32 off = ((struct Map *)gUnk_030052F4)->offsets[i & 3];
        s32 pos = start;
        for (j = 0; j < 4; j++) {
            pos += off;
            if (pos == target)
                return (192 + i * 64) & 0xff;
        }
    }
    return -1;
}
