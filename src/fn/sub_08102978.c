#include "global.h"

struct S { u8 filler[0xb6]; u16 h; u8 filler2[0xdc - 0xb8]; u32 k; };
extern u8 *gUnk_02000580;
void sub_08159404(u8 *, u32, u32);
void sub_08159424(u8 *, u32);

void sub_08102978(struct S *s)
{
    if ((u16)(s->h - 3) > 1) {
        sub_08159404(gUnk_02000580, 5, s->k);
    } else {
        sub_08159404(gUnk_02000580, 1, s->k);
        sub_08159424(gUnk_02000580, 4);
    }
}
