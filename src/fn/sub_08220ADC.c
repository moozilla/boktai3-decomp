#include "global.h"

extern u8 *gUnk_03001688;
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void *, void *);
void sub_08220A58(void);
void sub_08220AA0(void);
void sub_08220AAC(void);

u8 *sub_08220ADC(void) {
    u8 *p;
    if (gUnk_03001688 != 0)
        return gUnk_03001688;
    p = sub_08219FBC(0xb, 0xd4);
    if (p != 0) {
        sub_0821A04C(p, sub_08220A58, sub_08220AA0);
        gUnk_03001688 = p;
        sub_08220AAC();
    }
    return p;
}
