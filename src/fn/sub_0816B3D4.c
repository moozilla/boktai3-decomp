#define gSceneKeys gUnk_03005260
#include "global.h"
struct SceneInputRepeat { u8 unknown0[0xB]; u8 repeatTimer; };
extern volatile u16 gUnk_03005260[];
u32 SceneInput_ReadRepeatingDirections(struct SceneInputRepeat *s)
{
    u32 keys = gUnk_03005260[1];
    u32 mask = 0xF0;
    u32 pressed = mask & keys;
    if (pressed) {
        s->repeatTimer = 0;
        return gUnk_03005260[1];
    }
    if (gUnk_03005260[0] & mask) {
        if (++s->repeatTimer > 9) {
            s->repeatTimer = 0;
            return gUnk_03005260[0];
        }
    } else {
        s->repeatTimer = 0;
    }
    return 0;
}
