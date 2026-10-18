#define Hud_RegisterTimer sub_081783C0
#define gFieldHud gUnk_02000214
#include "global.h"
struct FieldHud { u8 unknown0[0xAFF]; u8 timerEnabled; u8 timerOption; u8 unknownB01[3]; void *timer; };
extern struct FieldHud *gUnk_02000214;
extern u32 gUnk_030053F4;
void sub_081783C0(void *timer)
{
    struct FieldHud *hud = gUnk_02000214;
    if (hud) {
        hud->timer = timer;
        hud->timerEnabled = 1;
        if (gUnk_030053F4 & 0x800)
            hud->timerOption = 1;
    }
}
