#include "global.h"

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);

void Menu_StartFadeOut(u32 a)
{
    if (a != 0) {
        ScreenTransition_SendCommand(1, a, 4, 4, 4, 0xFFFF, 0);
    }
}
