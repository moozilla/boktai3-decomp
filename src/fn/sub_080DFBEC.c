#include "global.h"

void SceneActorAnimation_SelectDirectionalEntry(void *, s32);

struct S {
    u8 filler[0x2b2];
    u8 f;
};

void sub_080DFBEC(struct S *s)
{
    if (s->f != 0)
        s->f = 0;
    SceneActorAnimation_SelectDirectionalEntry(s, 1);
}
