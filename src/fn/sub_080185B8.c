#include "global.h"
s32 sub_08003460(void);
void sub_08018438(u8 *);
void sub_08018454(u8 *, u8 *);
u32 sub_080185B8(u8 *p)
{
    u32 i;
    u8 *e;
    if (sub_08003460() != 0)
        sub_08018438(p);
    i = 0;
    if (i < *(u32 *)(p + 0x18)) {
        e = p + 0x21c;
        do {
            sub_08018454(p, e);
            e += 0x18;
            i++;
        } while (i < *(u32 *)(p + 0x18));
    }
    return 0;
}
