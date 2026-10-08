#include "global.h"

struct Buf { u32 a, b; u8 pad[0x180]; };
extern u32 gUnk_0300523C;
extern u32 gUnk_03001680;
struct Buf *sub_0821A2DC(void);

void sub_0821A308(void)
{
    struct Buf *b = sub_0821A2DC();
    if (gUnk_0300523C == 0) {
        struct Buf *e;
        gUnk_03001680 ^= 1;
        e = (struct Buf *)((u8 *)b + gUnk_03001680 * 0x188);
        e->a = 0;
        e->b = 0;
    }
}
