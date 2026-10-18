#include "global.h"
// Provisional object-family interpretation.
#define SceneTransition_StartSunScript sub_081C46D4
s32 SunClock_GetPhase(void);
void *ScriptProc_Start(u16, u16);
void *sub_081C46D4(void)
{
    s32 n = SunClock_GetPhase();
    u16 key;
    switch (n) {
    case 1: case 2: key = 0x2877; break;
    case 3: case 4: key = 0xe411; break;
    default: key = 0x225e; break;
    }
    return ScriptProc_Start(0x92b3, key);
}
