#include "global.h"
void CircleWindow_ResetState(void);
void VBlankIntrWait(void);
void Video_SetWindowBounds(int, int, int, int, int);
extern void *gUnk_0200009C;
int CircleWindow_Destroy(void)
{
    CircleWindow_ResetState();
    VBlankIntrWait();
    Video_SetWindowBounds(0, 0, 0, 240, 160);
    gUnk_0200009C = 0;
    return 0;
}
