#include "global.h"
#include "input.h"
void Vanargand_StopActionSequence(void *);

void sub_081ADBB8(u8 *p)
{
    if (gPadState[1] & 4) {
        s32 v = (p[0xab] + 1) % 32;
        if (v == 0)
            v = 1;
        Vanargand_StopActionSequence(p + 0x21c);
        p[0xaa] = v;
    }
}
