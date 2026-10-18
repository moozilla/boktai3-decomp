#include "global.h"

void SceneActor_LowerColorRedPeak(void);

struct S {
    u8 filler[0xcf0];
    void (*fp)(void);
    u8 f2[0xd54 - 0xcf4];
    u8 b;
};

s32 Ivy_WaitForColorRelease(struct S *s)
{
    s32 r;
    if (s->b != 0)
        r = 1;
    else {
        s->fp = SceneActor_LowerColorRedPeak;
        r = 0;
    }
    return r;
}
