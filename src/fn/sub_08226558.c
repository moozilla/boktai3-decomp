#include "global.h"
#include "engine.h"
#include "ram.h"

struct Camera {
    u8 pad[0x84];
    u8 b84;
    u8 b85;
    u16 h86;
    u16 h88;
    u16 h8A;
};

void Camera_SetSway(s32 a, s32 b, s32 mode)
{
    if (((struct Camera *)gActiveCamera) && !(gEngineFlags & 0x800)) {
        if (b == 0 && mode == 1) {
            ((struct Camera *)gActiveCamera)->b84 = b;
            ((struct Camera *)gActiveCamera)->b85 = b;
            ((struct Camera *)gActiveCamera)->h86 = b;
            ((struct Camera *)gActiveCamera)->h88 = b;
        } else {
            struct Camera *c;
            ((struct Camera *)gActiveCamera)->b84 = 1;
            c = ((struct Camera *)gActiveCamera);
            c->h88 = a;
            c->h8A = b;
            if ((u16)b == 0)
                c->h86 = c->h88;
            ((struct Camera *)gActiveCamera)->b85 = mode;
        }
    }
}
