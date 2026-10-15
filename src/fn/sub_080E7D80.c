#include "global.h"

s32 SceneActorProfiles_FindOrAdd(s32, s32);

struct S {
    u8 filler[0x98];
    u8 b;
    u8 f2[0x308 - 0x99];
    s32 v;
};

void sub_080E7D80(struct S *s)
{
    s->v = SceneActorProfiles_FindOrAdd(s->b, 0);
}
