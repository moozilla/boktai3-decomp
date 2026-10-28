#include "global.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"

struct PadState {
    u16 held;
    u16 pressed;
    u16 released;
    u16 pad;
};
extern struct PadState gPadState[2];

#define PAD_UPDATE(i, k)                                                                           \
    {                                                                                              \
        old = gPadState[i].held;                                                                   \
        gPadState[i].held = k;                                                                     \
        gPadState[i].pressed = k & ~old;                                                           \
        gPadState[i].released = old & ~k;                                                          \
    }

void sub_08219B24(void)
{
    s32 i;
    u16 keys, old;

    gHardwareKeys = *(vu16 *)0x04000130;
    if (gLinkMenuActive == 0) {
        keys = 0x3FF ^ gHardwareKeys;
        PAD_UPDATE(0, keys);
        for (i = 1; i < 2; i++) {
            keys = 0;
            PAD_UPDATE(i, keys);
        }
    } else if (!(gEnginePauseFlags & 1)) {
        for (i = 0; i < 2; i++) {
            if (gSceneSelectionDisplaySlots[i] == 0xFFFF)
                keys = 0;
            else
                keys = 0x3FF ^ gSceneSelectionDisplaySlots[i];
            PAD_UPDATE(i, keys);
        }
    }
}
