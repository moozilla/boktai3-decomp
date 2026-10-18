#include "global.h"
/* Solar_EnableControlFlag: Sets a nearby solar control flag whose precise purpose remains tentative */
#define Solar_EnableControlFlag sub_0822C180
extern u16 gUnk_030054B4;
void sub_0822C180(void)
{
    gUnk_030054B4 = 1;
}
