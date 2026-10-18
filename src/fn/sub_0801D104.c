#include "global.h"
void Blend_SetAlpha(int, int, int);
int ResultTutorial_FadeIn(u8 *p)
{
    int result;
    if (p[0xb5] != 16) {
        int v = (*(s16 *)(p + 0xaa) >> 1);
        Blend_SetAlpha(0, 8 - v, v + 8);
        p[0xb5] = v + 8;
        ++*(u16 *)(p + 0xaa);
        result = 0;
    } else
        result = 1;
    return result;
}
