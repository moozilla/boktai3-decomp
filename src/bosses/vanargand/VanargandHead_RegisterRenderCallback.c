#include "global.h"

void VanargandHead_OnTargetContact(void);
void TargetRegistry_RegisterSceneActor(u8 *, u8 *, void (*)(void));

void VanargandHead_RegisterRenderCallback(u8 *p)
{
    TargetRegistry_RegisterSceneActor(p + 0x250, p + 0x1FC, VanargandHead_OnTargetContact);
}
