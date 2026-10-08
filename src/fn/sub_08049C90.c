#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
void sub_08049C90(void)
{
    if (gUnk_02000488) {
        sub_08049168(gUnk_02000488, 0);
        *(u16 *)(gUnk_02000488 + 0x41C) = 1;
    }
}
