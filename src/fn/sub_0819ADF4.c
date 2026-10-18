#include "global.h"

void SceneOrbitHazard_SetState(u8 *, u32);

void SceneOrbitHazard_Activate(u8 *p)
{
    if (p != 0 && *(p + 0xE0) == 0) {
        SceneOrbitHazard_SetState(p, 1);
    }
}
