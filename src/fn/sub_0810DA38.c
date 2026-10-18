#include "global.h"

struct P {
    u8 f[0xd08];
    u32 v[7];
};
void TextWindow_Clear(u32);

void Registry3C9F_DestroyTextWindows(struct P *p)
{
    u32 *q;
    s32 i;
    TextWindow_Clear(p->v[0]);
    i = 0;
    q = &p->v[1];
    do {
        s32 n = i + 1;
        TextWindow_Clear(*q++);
        i = n;
    } while (i <= 5);
}
