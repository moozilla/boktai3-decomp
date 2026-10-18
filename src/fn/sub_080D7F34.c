#include "global.h"

s32 SceneActorProfiles_FindOrAdd(s32, s32);

struct S {
    u8 filler[0x308];
    s32 v;
};

void Mummy_InitProfile(struct S *s)
{
    s->v = SceneActorProfiles_FindOrAdd(1, 0);
}
