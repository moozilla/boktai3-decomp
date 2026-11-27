#include "global.h"
void Storage_GetSlotPosition(s16 *pos, s32 i)
{
    if (i <= 15) {
        pos[0] = (i & 3) * 24 + 0x70;
        pos[1] = (i >> 2) * 24 + 0x20;
    } else if (i <= 31) {
        s32 j = i - 16;
        pos[0] = (j & 3) * 24 + 8;
        pos[1] = (j >> 2) * 24 + 0x20;
    } else if (i == 32) {
        pos[0] = 0xd8;
        pos[1] = 0x68;
    } else if (i == 33) {
        pos[0] = 0xd8;
        pos[1] = 0x50;
    } else if (i == 34) {
        pos[0] = 0xd8;
        pos[1] = 0x38;
    }
}
