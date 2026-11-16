#include "global.h"

struct S080FAC14 {
    u8 filler[0x98];
    u8 a;
    u8 filler2[0x308 - 0x99];
    u32 b;
};
s32 SceneActorProfiles_FindOrAdd(u32, u32);

void Axe_InitProfile(struct S080FAC14 *s)
{
    s->b = SceneActorProfiles_FindOrAdd(s->a, 0);
}
