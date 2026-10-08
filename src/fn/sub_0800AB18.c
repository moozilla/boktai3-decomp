#include "global.h"

struct E { u8 act; u8 f[0x2f]; u8 k[0x118]; };
struct S { u8 f[0x28]; struct E e[6]; };
extern u32 gUnk_02000040;
void sub_08217EAC(void *);

s32 sub_0800AB18(struct S *s)
{
    s32 i;
    for (i = 0; i <= 5; i++) {
        struct E *e = &s->e[i];
        if (e->act != 0) {
            u8 (*k)[0x50] = (u8 (*)[0x50])e->k;
            s32 j;
            for (j = 3; j >= 0; j--) {
                sub_08217EAC(k);
                k++;
            }
        }
    }
    {
        u32 z = 0;
        gUnk_02000040 = z;
    }
    return 0;
}
