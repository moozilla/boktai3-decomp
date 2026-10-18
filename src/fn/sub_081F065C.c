#include "global.h"

void sub_081F05D0(void);
void TargetRegistry_RegisterSceneActor(u8 *, u8 *, void (*)(void));

void SingletonSlot7_RegisterRenderCallback(u8 *p)
{
    TargetRegistry_RegisterSceneActor(p + 0x250, p + 0x1FC, sub_081F05D0);
}
