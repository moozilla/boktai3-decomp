#include "global.h"
void sub_081EC388(u8 *, u32);
void SceneActors_EmitPeriodicParticles(u8 *a, u32 b)
{
    u32 m = 0xf;
    if ((b & m) == 0)
        sub_081EC388(a, 0);
    if (((b + 3) & m) == 0)
        sub_081EC388(a, 1);
}
