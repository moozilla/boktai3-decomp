#include "global.h"
#include "engine.h"
void *sub_0802140C(u32 slot);
/* Script: for the actor in singleton slot 7 (registry 5856): +0x5DC = -t
   (default 128), +0x5A6 = 3, +0x5EA = 0, +0x5EC = 1 (tentative). */
void SingletonSlot7_SetTimerFromScript(void)
{
    s32 t = 0x80;
    u8 *p = sub_0802140C(7);
    if (p) {
        u16 zero;
        u16 *timer;
        if (Script_SeekToKeyword('t'))
            t = Script_GetValue();
        timer = (u16 *)(p + 0x5DC);
        zero = 0;
        *timer = t;
        *(u16 *)(p + 0x5A6) = 3;
        *(u16 *)(p + 0x5EA) = zero;
        *(u16 *)(p + 0x5EC) = 1;
    }
}
