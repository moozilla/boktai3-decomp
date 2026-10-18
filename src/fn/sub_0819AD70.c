#include "global.h"

void sub_081805C8(u8 *);
void sub_08180654(u8 *);
void SceneOrbitHazard_SetState(u8 *, u32);

u32 SceneOrbitHazard_Init(u8 *p)
{
    u8 *q = p + 0x18;
    sub_081805C8(q);
    sub_08180654(q);
    SceneOrbitHazard_SetState(p, 0);
    return 0;
}
