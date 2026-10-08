#include "global.h"
struct O { u8 pad[0x58]; s16 h; u8 pad2[0xb0-0x5a]; u32 cb; u8 pad3[0xbf-0xb4]; u8 b; };
void sub_0811ED0C(void);
void sub_0811EDE0(struct O *o)
{
    if (o->b == 0 && o->h > 200) {
        u8 *p = (u8 *)o + 0xb0;
        *(u32 *)p = (u32)sub_0811ED0C;
        p[0xe] = 1;
    }
}
