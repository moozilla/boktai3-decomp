#include "global.h"

void sub_08232264(u8 *, u8 *, u32, u32, u32);

void SceneActorEffects_InitAtActorPosition(u8 *p)
{
    sub_08232264(p + 0x47c, p + 0x5c, 0, 0, 0);
}
