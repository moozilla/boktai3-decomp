#include "global.h"

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
void Registry2388_SetUpdate(void *, void (*)(void));
void Registry2388_UpdateScriptDelay(void);

void Registry2388_BeginFadeOut(void *p)
{
    ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0xFFFF);
    Registry2388_SetUpdate(p, Registry2388_UpdateScriptDelay);
}
