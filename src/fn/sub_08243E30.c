#include "global.h"
extern u32 gUnk_030035C0;
void Solar_ShutdownTimer(void);
void Solar_Disable(void)
{
    if (gUnk_030035C0 != 0) {
        gUnk_030035C0 = 0;
        Solar_ShutdownTimer();
    }
}
