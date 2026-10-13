#include "global.h"

struct Q { u8 pad[0x30]; u32 f30; u8 pad2[4]; u8 f38; };
struct P { u8 pad[0x2314]; u32 a[2]; u32 b[2]; };

u32 sub_0803D210(struct P *p, s32 i, struct Q *q)
{
    u32 v;
    switch (q->f38) {
    case 0:
        v = p->a[i];
        if (v > 0x18FFFF) {
            if (q->f30 <= v - 0x64000 && q->f30 >= v - 0xC8000)
                return 1;
        }
        break;
    case 1:
        v = p->b[i];
        if (q->f30 >= v + 0x64000 && q->f30 <= v + 0xC8000)
            return 1;
        break;
    }
    return 0;
}
