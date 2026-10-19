#include "global.h"

struct Ent {
    s32 state;
    s32 pad4;
    s32 id;
    u8 rest[0x908 - 12];
};

u32 sub_08120FF4(s32 id)
{
    u32 t = *(u32 *)0x0200020C;

    if (t != 0) {
        u32 b = t;
        s32 i = 0;
        u32 off = 0;
        struct Ent *p = (struct Ent *)(b + 0x8CC);
        do {
            if (p->state == 1 && p->id == id)
                return t + (off + 0x18);
            p++;
            off += 0x908;
            i++;
        } while (i < 1);
    }
    return 0;
}
