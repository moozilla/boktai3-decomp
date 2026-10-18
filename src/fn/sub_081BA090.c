#include "global.h"

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Registry2388_SetUpdate(void *, void (*)(void));
void Registry2388_UpdateOpeningDelay(void);

void Registry2388_BeginFadeIn(void *p)
{
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    Registry2388_SetUpdate(p, Registry2388_UpdateOpeningDelay);
}
