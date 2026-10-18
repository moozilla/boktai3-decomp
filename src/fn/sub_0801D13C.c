#include "global.h"
void Blend_SetAlpha(int, int, int);
int ResultTutorial_FadeOut(u8 *p)
{
    int result;
    if (p[0xb5] != 8) {
        int v = (*(s16 *)(p + 0xaa) >> 1);
        int alpha = 16 - v;
        Blend_SetAlpha(0, v, alpha);
        p[0xb5] = alpha;
        ++*(u16 *)(p + 0xaa);
        result = 0;
    } else
        result = 1;
    return result;
}
