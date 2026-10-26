#include "global.h"
#include "ram.h"
void Solar_InitControllerState(void *);
void sub_0822CB08(void *);
u32 Solar_InitController(void *p)
{
    Solar_InitControllerState(p);
    sub_0822CB08(p);
    gSolarController = p;
    return 0;
}
