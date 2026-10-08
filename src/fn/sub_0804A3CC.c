#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
void sub_08049364(u8 *);
void sub_0804A3CC(void)
{
    u8 *p = gUnk_02000488;
    if (p) {
        if (p[0x255] != 5) {
            if (p[0x3E4] == 0)
                sub_08049364(p);
        }
    }
}
