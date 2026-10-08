#include "global.h"

u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
s32 sub_080FE68C(u8 *);
void sub_080FE668(u8 *);
void sub_08219D38(u8 *);

void sub_080FF754(void)
{
    u8 *p = sub_08219C40(0xf24);
    if (p != 0) {
        sub_08219DD8(p, 0xf24);
        if (sub_080FE68C(p) < 0) {
            sub_080FE668(p);
            sub_08219D38(p);
        }
    }
}
