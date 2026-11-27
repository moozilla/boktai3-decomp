#include "global.h"
#include "engine/runtime.h"
/* Solar_DisableControlFlag: Clears the same tentative solar control flag */
void Solar_DisableControlFlag(void)
{
    gSolarControlFlag = 0;
}
