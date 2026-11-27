#include "global.h"
struct Object {
    u32 w0, w4, w8, wc, w10, w14;
    void *w18;
    u32 w1c, w20, w24, w28, w2c;
};
void *CameraFollow_ConvertDistanceScale(void *, u32);
void CameraFollow_Initialize(struct Object *p, u32 a, u32 b, u32 c)
{
    p->w0 = a;
    p->w4 = b;
    p->w8 = 0;
    p->wc = 1;
    p->w10 = 0;
    p->w14 = 256;
    p->w18 = CameraFollow_ConvertDistanceScale(p, 0x1000);
    p->w1c = c;
    p->w20 = 0;
    p->w24 = 0;
    p->w28 = 0;
    p->w2c = 32;
}
