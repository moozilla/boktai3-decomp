#include "global.h"
void Crow_StopAtRaisedTerrain(void);
u32 Crow_UpdatePostAction(void)
{
    Crow_StopAtRaisedTerrain();
    return 1;
}
