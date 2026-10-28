#include "global.h"
#include "ram.h"
void RegistryF3E4_StartDisappearing(u32);
void RegistryF3E4_Hide(void)
{
    RegistryF3E4_StartDisappearing(gRegistryF3E4Actor);
}
