#include "global.h"
#include "ram.h"
void Effect_Remove(u8 *);

void sub_08014C28(void)
{
    u8 *p = gParticleGroupManager;
    if (p != 0) {
        s32 i = 0;
        u32 z = 0;
        do {
            u8 *e = p + 0x24 + i * 0x1c4;
            u32 t = e[0];
            s32 next = i + 1;
            if (t != 0) {
                u8 *b = e + 4;
                u8 *a = e + 0x14;
                s32 k = 7;
                do {
                    if (*b != 0) {
                        Effect_Remove(a);
                        *b = z;
                    }
                    b += 0x38;
                    a += 0x38;
                    k--;
                } while (k >= 0);
                e[0] = z;
            }
            i = next;
        } while (i <= 5);
    }
}
