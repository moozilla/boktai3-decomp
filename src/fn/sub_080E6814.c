#include "global.h"

u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
s32 sub_080E5744(u8 *);
void sub_080E5738(u8 *);
void sub_08219D38(u8 *);

void sub_080E6814(void)
{
    u8 *p = sub_08219C40(0x6e0);
    if (p != 0) {
        sub_08219DD8(p, 0x6e0);
        if (sub_080E5744(p) < 0) {
            sub_080E5738(p);
            sub_08219D38(p);
        }
    }
}
