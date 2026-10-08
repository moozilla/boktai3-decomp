#include "global.h"

struct Slot { u16 id; u8 flags; u8 pad; u32 val; };
struct Slot *sub_0821A1C8(u16);

void sub_0821A2AC(u16 id)
{
    struct Slot *s = sub_0821A1C8(id);
    if (s != 0)
        s->flags = 0;
}
