#include "global.h"
struct S {
    u8 f[0x11CC];
    u32 a;
    u32 b;
    u32 c;
};
u32 TextWindow_Create(s32, s32, s32, s32);
void StageResults_CreateTextWindows(struct S *s)
{
    s->a = TextWindow_Create(1, 3, 0xa, 2);
    s->b = TextWindow_Create(0xd, 3, 4, 2);
    s->c = TextWindow_Create(0xd, 7, 0x10, 2);
}
