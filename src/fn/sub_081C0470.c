#include "global.h"
struct S {
    u8 f[0x30];
    u32 a;
    u32 b;
};
void ScenePaletteFade_SetState(struct S *p, u32 v)
{
    p->a = v;
    p->b = 0;
}
