#include "global.h"
struct P { u8 pad[0x18]; u8 a; u8 b; u8 c; u8 pad2[5]; u32 d; };
s32 sub_08036AE0(void);
void sub_08037A6C(struct P *p)
{
    s32 r = sub_08036AE0();
    u32 v;
    if (p->b != 7) {
        if (r != p->a) {
            switch (r) {
            case 27: {
                u32 one = 1;
                u32 z = 0;
                p->b = one;
                p->c = one;
                p->d = z;
                break;
            }
            case 7: v = 2; goto s1;
            case 9: v = 3; goto s1;
            case 11: v = 4; goto s1;
            case 28: v = 5; goto s1;
            case 29: v = 6;
            s1:
                p->b = v;
                p->c = 1;
                p->d = 0;
                break;
            default: break;
            }
        }
        p->a = r;
    }
}
