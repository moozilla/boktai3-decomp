#include "global.h"
extern u32 gUnk_03006A34, gUnk_030035C0;
void Solar_InitTimer(void);
void Solar_Enable(void)
{
    gUnk_03006A34 = 0;
    gUnk_030035C0 = 1;
    Solar_InitTimer();
}
