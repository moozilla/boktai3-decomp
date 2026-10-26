#include "global.h"
#include "ram.h"
void CircleWindow_ResetState(void);
void VBlankIntrWait(void);
void Video_SetWindowBounds(int, int, int, int, int);
int CircleWindow_Destroy(void)
{
    CircleWindow_ResetState();
    VBlankIntrWait();
    Video_SetWindowBounds(0, 0, 0, 240, 160);
    gCircleWindow = 0;
    return 0;
}
