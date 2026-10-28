#include "global.h"
struct SceneInputRepeat {
    u8 unknown0[0xB];
    u8 repeatTimer;
};
extern volatile u16 gPadState[];
u32 SceneInput_ReadRepeatingDirections(struct SceneInputRepeat *s)
{
    u32 keys = gPadState[1];
    u32 mask = 0xF0;
    u32 pressed = mask & keys;
    if (pressed) {
        s->repeatTimer = 0;
        return gPadState[1];
    }
    if (gPadState[0] & mask) {
        if (++s->repeatTimer > 9) {
            s->repeatTimer = 0;
            return gPadState[0];
        }
    } else {
        s->repeatTimer = 0;
    }
    return 0;
}
