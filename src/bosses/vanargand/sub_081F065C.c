#include "global.h"

void SingletonSlot7_OnTargetContact(void);
void TargetRegistry_RegisterSceneActor(u8 *, u8 *, void (*)(void));

void SingletonSlot7_RegisterRenderCallback(u8 *p)
{
    TargetRegistry_RegisterSceneActor(p + 0x250, p + 0x1FC, SingletonSlot7_OnTargetContact);
}
