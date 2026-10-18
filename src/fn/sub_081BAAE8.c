#include "global.h"

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Registry3F74_SetState(void *, void (*)(void));
void Registry3F74_UpdateScriptDelay(void);

void Registry3F74_StartClosingFade(void *p)
{
    ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0);
    Registry3F74_SetState(p, Registry3F74_UpdateScriptDelay);
}
