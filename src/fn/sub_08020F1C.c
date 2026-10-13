#include "global.h"
#define SceneMessages_ConsumeSignal sub_08020F1C
extern u8 *gUnk_0200047C;
void sub_08020D28(void);
int SceneMessages_ConsumeSignal(void)
{
    u8 *p = gUnk_0200047C;
    if (!p || !*(int *)(p + 0x70))
        return -1;
    *(int *)(p + 0x70) = 0;
    sub_08020D28();
    return 0;
}
