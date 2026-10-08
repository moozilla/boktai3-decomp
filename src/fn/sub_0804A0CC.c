#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
u32 sub_0805C760(u8 *);
u32 sub_0804A0CC(u32 a, u32 b)
{
    if (gUnk_02000488) {
        if (!sub_0805C760(gUnk_02000488)) {
            if (gUnk_02000488[0x1A] == 9) {
                gUnk_02000488[0x240] = 1;
                gUnk_02000488[0x241] = b;
                *(u16 *)(gUnk_02000488 + 0x242) = a;
                return 1;
            }
        }
    }
    return 0;
}
