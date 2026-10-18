#include "global.h"

void SceneActorEffects_AdvanceSteps(void *, s32);
void Memory_Clear(void *, s32);

struct S {
    u8 filler[0x5b8];
    u8 a[8];
    u8 filler2[2];
    u16 h5c2;
    u16 h5c4;
};
struct S2 {
    u8 filler[0x5c0];
    u16 h5c0;
    u16 h5c2;
};

void Mummy_ResetMotionState(struct S2 *s)
{
    SceneActorEffects_AdvanceSteps(s, 4);
    Memory_Clear((u8 *)s + 0x5b8, 8);
    s->h5c2 = 0;
    s->h5c0 = 0;
}
