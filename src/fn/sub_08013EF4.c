#include "global.h"
struct P { u32 a, b; };
void *sub_08013C14(void);
u8 *sub_08013C2C(void *, u32);
s32 sub_08013EF4(u32 a, struct P *b)
{
    void *p = sub_08013C14();
    u8 *q;
    if (p != 0) {
        q = sub_08013C2C(p, a);
        if (q != 0) {
            *(struct P *)(q + 4) = *b;
            return 0;
        }
    }
    return -1;
}
