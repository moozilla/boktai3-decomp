#define gMenuKeys                     gUnk_03005260
#include "global.h"
struct MenuInputController {
    u8 unknown[0x43e8];
    u8 repeatTimer;
};
extern volatile u16 gMenuKeys[];
u32 MenuInput_ReadDirectionRepeat(struct MenuInputController *p)
{
    u32 result;
    u32 pressed = gMenuKeys[1] & 0xf0;
    if (pressed) {
        u32 offset = 0x43e8;
        u8 *timer = (u8 *)p + offset;
        *timer = 0;
        result = gMenuKeys[1];
        goto done;
    }
    if (gMenuKeys[0] & 0xf0) {
        if (++p->repeatTimer > 9) {
            p->repeatTimer = pressed;
            result = gMenuKeys[0];
            goto done;
        }
    }
    result = 0;
done:
    return result;
}
