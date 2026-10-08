#include "global.h"

struct Buf { u32 a, b; u8 pad[0x180]; };
extern u32 gUnk_03001680;
struct Buf *sub_0821A2DC(void);

void sub_0821A2E4(void)
{
    struct Buf *b = sub_0821A2DC();
    gUnk_03001680 = 0;
    b[0].a = 0;
    b[0].b = 0;
    b[1].a = 0;
    b[1].b = 0;
}
