#include "global.h"
extern void *gUnk_03002604;
void Solar_InitControllerState(void *);
void sub_0822CB08(void *);
u32 Solar_InitController(void *p)
{
    Solar_InitControllerState(p);
    sub_0822CB08(p);
    gUnk_03002604 = p;
    return 0;
}
