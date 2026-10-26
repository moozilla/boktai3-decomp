#include "global.h"
#include "ram.h"

void RegistryF246_RemoveAnimations(void);

s32 RegistryF246_Destroy(void)
{
    RegistryF246_RemoveAnimations();
    gLinkPanel = 0;
    return 1;
}
