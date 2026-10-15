#include "global.h"
void SceneActorAnimation_SelectProfileEntry(u8 *, u32);
void sub_0808E54C(u8 *p)
{
    if (p[0x2b2] != 0)
        p[0x2b2] = 0;
    SceneActorAnimation_SelectProfileEntry(p, 1);
}
