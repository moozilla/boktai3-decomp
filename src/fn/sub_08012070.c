#include "global.h"
struct Vec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void sub_082144A4(void *, void *, int);
void sub_0821FEB4(void *, int, int, int, int, void *, void *);
void sub_0821FF84(void *, void *, void *);
void sub_0821FF58(void *, int, int, int, int, int);
void sub_08220C8C(void *, void *, int, int, int);
#define AnimatedHazard_Initialize sub_08012070
void AnimatedHazard_Initialize(u8 *m, u8 *p)
{
    struct Vec size, offset;
    int zero;
    u8 *sprite = p + 0x64, *effect;
    sub_082144A4(sprite, m + 0x18, 1);
    zero = 0;
    sprite[7] = 2;
    sub_08220C8C(p + 0x90, *(void **)(m + 0x34), 0, 0, zero);
    effect = p + 0x10;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    sub_0821FEB4(effect, 0, 0x2001, 0, 16, &size, &offset);
    sub_0821FF84(effect, 0, p);
    sub_0821FF58(effect, 100, 50, 0, 0x60000, 30);
    p[0] = zero;
}
