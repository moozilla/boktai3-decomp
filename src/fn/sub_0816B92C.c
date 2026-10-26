#include "global.h"
#include "ram.h"
void MapScreen_SuppressSpecialMarkers(void)
{
    if (gFieldHud)
        *(u32 *)(gFieldHud + 0x4928) = 1;
}
