#include "global.h"
/* Solar_DisableControlFlag: Clears the same tentative solar control flag */
#define Solar_DisableControlFlag sub_0822C18C
extern u16 gUnk_030054B4;
void sub_0822C18C(void)
{
    gUnk_030054B4 = 0;
}
