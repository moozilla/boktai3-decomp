#include "global.h"
extern u8 *gUnk_0200047C;
void SceneMessages_RequestAdvance(void);
int SceneMessages_ConsumeSignal(void)
{
    u8 *p = gUnk_0200047C;
    if (!p || !*(int *)(p + 0x70))
        return -1;
    *(int *)(p + 0x70) = 0;
    SceneMessages_RequestAdvance();
    return 0;
}
