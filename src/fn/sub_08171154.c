#include "global.h"
s32 sub_0822E104(u32);
u8 *sub_0822E288(s32);
void sub_08171154(u32 a, u16 *t)
{
    u8 *e;
    s32 i;
    if (sub_0822E104(a) != -1) {
        e = sub_0822E288(sub_0822E104(a));
        i = 0;
        do {
            u32 k, v;
            if (i == 0) {
                k = e[1];
                v = e[2];
            } else {
                k = e[3];
                v = e[4];
            }
            switch (k) {
            case 1:
                t[1] += v;
                break;
            case 2:
                t[2] += v;
                break;
            case 3:
                t[2] += v;
            case 4: case 5: case 6:
                t[0] += v;
                break;
            }
            i++;
        } while (i <= 1);
    }
}
