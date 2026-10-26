#include "global.h"
#include "render.h"

void Video_SetWindowBounds(u32 n, u32 a, u32 b, u32 c, u32 d)
{
    if (n == 0) {
        gWindowShadow0 = (a << 8) | c;
        *(vu16 *)0x04000040 = gWindowShadow0;
        gWindowShadow2 = (b << 8) | d;
        *(vu16 *)0x04000044 = gWindowShadow2;
    } else {
        gWindowShadow3 = (a << 8) | c;
        *(vu16 *)0x04000042 = gWindowShadow3;
        gWindowShadow1 = (b << 8) | d;
        *(vu16 *)0x04000046 = gWindowShadow1;
    }
}
