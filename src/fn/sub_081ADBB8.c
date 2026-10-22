#include "global.h"
extern u16 gUnk_03005260[];
void Vanargand_StopActionSequence(void *);

void sub_081ADBB8(u8 *p)
{
    if (gUnk_03005260[1] & 4) {
        s32 v = (p[0xab] + 1) % 32;
        if (v == 0)
            v = 1;
        Vanargand_StopActionSequence(p + 0x21c);
        p[0xaa] = v;
    }
}
