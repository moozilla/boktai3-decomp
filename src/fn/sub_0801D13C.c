#include "global.h"
#define ResultTutorial_FadeOut sub_0801D13C
void sub_08219574(int, int, int);
int ResultTutorial_FadeOut(u8 *p)
{
    int result;
    if (p[0xb5] != 8) {
        int v = (*(s16 *)(p + 0xaa) >> 1);
        int alpha = 16 - v;
        sub_08219574(0, v, alpha);
        p[0xb5] = alpha;
        ++*(u16 *)(p + 0xaa);
        result = 0;
    } else
        result = 1;
    return result;
}
