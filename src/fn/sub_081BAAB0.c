#include "global.h"

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Registry3F74_SetState(void *, void (*)(void));
void Registry3F74_UpdateOpeningDelay(void);

void Registry3F74_StartOpeningFade(void *p)
{
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    Registry3F74_SetState(p, Registry3F74_UpdateOpeningDelay);
}
