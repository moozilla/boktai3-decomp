#include "global.h"
extern u8 *gUnk_0200048C;
void sub_080508F0(u8 *);
void sub_08050878(void)
{
    u8 *g = gUnk_0200048C;
    if (g != 0) {
        if (g[0x1f] == 0) {
            sub_080508F0(g);
            g[0x1f] = 1;
        }
    }
}
