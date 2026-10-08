#include "global.h"
extern u32 gUnk_02000064;
void sub_08214514(u8 *);
u32 sub_08015074(u8 *p)
{
    s32 i = 0;
    do {
        u8 *e = p + 0x20 + i * 0x178;
        u32 t = e[0];
        s32 next = i + 1;
        if (t != 0) {
            s32 j = 0;
            u8 *a = e + 0x2c;
            do {
                if (a[j] != 0) sub_08214514(e + 0x70 + j * 0x2c);
                j++;
            } while (j <= 5);
        }
        i = next;
    } while (i <= 3);
    return gUnk_02000064 = 0;
}
