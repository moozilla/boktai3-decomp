#include "global.h"
#include "engine.h"

struct Camera {
    u8 pad[0x84];
    u8 b84;
    u8 b85;
    u16 h86;
    u16 h88;
    u16 h8A;
};

extern struct Camera *gActiveCamera;

void Camera_SetSway(s32 a, s32 b, s32 mode)
{
    if (gActiveCamera && !(gEngineFlags & 0x800)) {
        if (b == 0 && mode == 1) {
            gActiveCamera->b84 = b;
            gActiveCamera->b85 = b;
            gActiveCamera->h86 = b;
            gActiveCamera->h88 = b;
        } else {
            struct Camera *c;
            gActiveCamera->b84 = 1;
            c = gActiveCamera;
            c->h88 = a;
            c->h8A = b;
            if ((u16)b == 0)
                c->h86 = c->h88;
            gActiveCamera->b85 = mode;
        }
    }
}
