#include "global.h"
s32 GameState_GetUnlockEntry(s32);
void sub_0822E764(s32, s32);
void sub_0822E86C(s32 first)
{
    s32 count = 0, gaps = 0, i, j;

    for (i = 0; i < 12; i++) {
        if (GameState_GetUnlockEntry(first + i) == 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                sub_0822E764(first + i, first + i - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (GameState_GetUnlockEntry(first + i) > GameState_GetUnlockEntry(first + j))
                    sub_0822E764(first + i, first + j);
            }
    }
}
