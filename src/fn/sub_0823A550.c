#define ActorEffect_TestStatusFlags sub_0823A550
#include "global.h"
struct ActorEffectStatus { u8 unknown0[0x38]; u32 flags; };
s32 sub_0823A550(void *unused0, void *unused1, struct ActorEffectStatus *s)
{
    u32 mask = 0x80;
    if (s->flags & mask) goto hit;
    if (s->flags & 0x100) goto hit;
    if (s->flags & 0x200) goto hit;
    if (s->flags & 0x400) goto hit;
    if (!(s->flags & 0x800)) goto clear;
hit:
    return 10;
clear:
    return 0;
}
