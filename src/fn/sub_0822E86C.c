#include "global.h"
s32 GameState_GetUnlockEntry(u8 *);
void sub_0822E764(u8 *, u8 *);
void sub_0822E86C(u8 *p)
{
    s32 count = 0, gaps = 0, i, j;
    for (i = 0; i < 12; i++) {
        u8 *q = p + i;
        if (GameState_GetUnlockEntry(q) == 0)
            gaps++;
        else {
            count++;
            if (gaps > 0)
                sub_0822E764(q, q - gaps);
        }
    }
    if (count > 1) {
        for (i = 0; i < count - 1; i++)
            for (j = i; j < count; j++) {
                if (GameState_GetUnlockEntry(p + i) > GameState_GetUnlockEntry(p + j))
                    sub_0822E764(p + i, p + j);
            }
    }
}
