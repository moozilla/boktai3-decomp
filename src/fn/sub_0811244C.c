#include "global.h"

struct S {
    u8 filler[0x000011CA];
    u8 f;
};

void ScriptDisplay_OnTransitionEnd(struct S *s)
{
    if (s->f != 0)
        s->f = 0;
}
