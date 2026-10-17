#include "global.h"

struct Camera {
    u8 pad[0x84];
    u8 b84;
    u8 b85;
    u16 h86;
    u16 h88;
    u16 h8A;
};

extern struct Camera *gUnk_03005420;
extern u32 gUnk_030053F4;

void Camera_SetSway(s32 a, s32 b, s32 mode)
{
    if (gUnk_03005420 && !(gUnk_030053F4 & 0x800)) {
        if (b == 0 && mode == 1) {
            gUnk_03005420->b84 = b;
            gUnk_03005420->b85 = b;
            gUnk_03005420->h86 = b;
            gUnk_03005420->h88 = b;
        } else {
            struct Camera *c;
            gUnk_03005420->b84 = 1;
            c = gUnk_03005420;
            c->h88 = a;
            c->h8A = b;
            if ((u16)b == 0)
                c->h86 = c->h88;
            gUnk_03005420->b85 = mode;
        }
    }
}
