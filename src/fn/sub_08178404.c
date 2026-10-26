#include "global.h"
#include "ram.h"
struct FieldHud {
    u8 unknown0[0xAFF];
    u8 timerEnabled;
    u8 timerOption;
    u8 unknownB01[3];
    void *timer;
    u8 unknownB08[8];
    u32 timerSpriteFlags0;
    u8 unknownB14[0x5C];
    u32 timerSpriteFlags1;
    u8 unknownB74[0x5C];
    u32 timerSpriteFlags2;
};
void Hud_ClearTimer(void)
{
    struct FieldHud *hud = gFieldHud;
    if (hud) {
        hud->timer = 0;
        hud->timerSpriteFlags0 |= 1;
        hud->timerSpriteFlags1 |= 1;
        hud->timerSpriteFlags2 |= 1;
        hud->timerEnabled = 0;
        hud->timerOption = 0;
    }
}
