#include "global.h"
extern const s32 gUnk_0861407C[];
s32 Lookup_FindValueIndex(s32 unused, s32 value)
{
    s32 result = -1;
    s32 i;
    if (value <= 9) {
        for (i = 0; i <= 9; i++) {
            if (value == gUnk_0861407C[i]) {
                result = i;
                break;
            }
        }
    }
    return result;
}
