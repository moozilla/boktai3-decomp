#include "global.h"
#include "engine.h"
struct Map {
    u8 pad[0x1c];
    s16 offsets[4];
};
s32 sub_0821BCE4(s32 start, s32 target)
{
    s32 i, j;
    for (i = 0; i < 4; i++) {
        s32 off = ((struct Map *)gTerrainMap)->offsets[i & 3];
        s32 pos = start;
        for (j = 0; j < 4; j++) {
            pos += off;
            if (pos == target)
                return (192 + i * 64) & 0xff;
        }
    }
    return -1;
}
