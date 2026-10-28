#include "global.h"
#include "runtime.h"
/* Solar_DisableControlFlag: Clears the same tentative solar control flag */
#define Solar_DisableControlFlag sub_0822C18C
void sub_0822C18C(void)
{
    gSolarControlFlag = 0;
}
