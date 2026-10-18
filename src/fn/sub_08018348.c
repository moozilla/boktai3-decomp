#include "global.h"
extern u8 *gUnk_02000090;
int Weather_EnterPhase(void *, int);
int Weather_Start(void)
{
    u8 *p = gUnk_02000090;
    if (!p)
        return -1;
    Weather_EnterPhase(p, 1);
    p[32] = 1;
}
