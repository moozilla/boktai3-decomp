#include "global.h"
#include "render.h"
void Bg_UpdateScrollShadow(void)
{
    u16 *dst = gBgScrollShadow;
    struct BackgroundLayer *src = gBgRecords;
    u32 mask;
    dst[0] = ((u16)src[0].x) & 0xff;
    mask = 0xff;
    dst[1] = mask & ((u16)src[0].y);
    dst[2] = mask & ((u16)src[1].x);
    dst[3] = mask & ((u16)src[1].y);
    if (!gBgAffineMode) {
        dst[4] = mask & ((u16)src[2].x);
        dst[5] = mask & ((u16)src[2].y);
        dst[6] = mask & *(u16 *)((u8 *)src + 0xb0);
        dst[7] = mask & *(u16 *)((u8 *)src + 0xb2);
    } else {
        u16 *a = gBgAffineMatrixBuffer, *b = gBgAffineMatrix;
        u32 *c;
        a[0] = b[0];
        a[1] = b[1];
        a[2] = b[2];
        a[3] = b[3];
        c = gBgAffineRefBuffer;
        c[0] = gBgAffineRefX;
        c[1] = gBgAffineRefY;
    }
}
