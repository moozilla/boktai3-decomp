#include "global.h"
extern u8 *gGameState;

bool32 sub_0822E6E4(s32 index)
{
    s32 count = 0;
    s32 i;
    bool32 result;
    for (i = 0; i < 8; i++) {
        if (*(s16 *)(gGameState + index * 2 + 0x78) > 0)
            count++;
    }
    if (count > 0) {
        *(s16 *)(gGameState + index * 2 + 0x78) = -1;
        result = TRUE;
    } else {
        result = FALSE;
    }
    return result;
}
