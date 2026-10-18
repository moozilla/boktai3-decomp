#include "global.h"

u32 ResourceRegistry_UnregisterResource(u32);

u32 GroundEffect_DestroyManager(void)
{
    ResourceRegistry_UnregisterResource(0xF0DE);
    return 0;
}
