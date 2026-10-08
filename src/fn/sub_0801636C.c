#include "global.h"
extern u32 gUnk_0200006C;
void sub_08217EAC(u8 *);
u32 sub_0801636C(u8 *p)
{
    s32 i = 0;
    do {
        u8 *e = p + 0x24 + i * 0x224;
        u32 t = e[0];
        s32 next = i + 1;
        if (t != 0) {
            s32 j = 0;
            u8 *q;
            if (j < e[1]) {
                q = e + 0x10;
                do {
                    sub_08217EAC(q);
                    q += 0x44;
                    j++;
                } while (j < e[1]);
            }
        }
        i = next;
    } while (i <= 5);
    { u32 z = 0; gUnk_0200006C = z; }
    return 0;
}
