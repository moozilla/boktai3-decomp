#include "global.h"
#include "engine.h"
#include "ram.h"
struct FieldHud {
    u8 unknown0[0xAFF];
    u8 timerEnabled;
    u8 timerOption;
    u8 unknownB01[3];
    void *timer;
};
void Hud_RegisterTimer(void *timer)
{
    struct FieldHud *hud = gFieldHud;
    if (hud) {
        hud->timer = timer;
        hud->timerEnabled = 1;
        if (gEngineFlags & 0x800)
            hud->timerOption = 1;
    }
}
