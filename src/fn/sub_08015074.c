#include "global.h"
#include "ram.h"
void Sprite_Remove(u8 *);
u32 FragmentManager_Destroy(u8 *p)
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
                if (a[j] != 0)
                    Sprite_Remove(e + 0x70 + j * 0x2c);
                j++;
            } while (j <= 5);
        }
        i = next;
    } while (i <= 3);
    return gFragmentManager = 0;
}
