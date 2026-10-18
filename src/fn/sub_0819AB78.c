#include "global.h"

void SceneOrbitHazard_SetState(u8 *, u32);

void SceneOrbitHazard_TransitionIfComplete(u8 *p, u32 v)
{
    if (*(p + 0xE1) == 0xff) {
        SceneOrbitHazard_SetState(p, v);
    }
}
