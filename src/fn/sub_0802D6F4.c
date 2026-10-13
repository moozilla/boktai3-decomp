#include "global.h"
extern void *gUnk_02000480;
void *sub_08219C40(int);
void sub_08219DD8(void *, int);
void *sub_0802D6F4(int unused, int variant)
{
    if (gUnk_02000480) {
        int size = 0x384;
        u8 *p = sub_08219C40(size);
        if (p) {
            sub_08219DD8(p, size);
            p[1] = variant;
            return p;
        }
    }
    return 0;
}
