#include "global.h"
#include "engine.h"
#include "ram.h"

void Camera_RequestShake(s32 a)
{
    u8 *s = gActiveCamera;
    if (s != 0) {
        if ((gEngineFlags & 0x800) == 0) {
            u16 *q = (u16 *)(s + 0x7e);
            if (a > *q)
                *q = a;
        }
    }
}
