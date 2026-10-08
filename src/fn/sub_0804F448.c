#include "global.h"
struct Q { u32 a; u8 f[0xc]; u16 s; u8 g[0xa]; u16 x, y, z; };
void sub_082151E4(u8 *, u32);
void sub_082144A4(struct Q *, u8 *, u32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0804F448(u8 *p)
{
    struct Q *q = (struct Q *)(p + 0x18);
    u8 *r4 = p + 0x44;
    s32 r;
    sub_082151E4(r4, 0xa47c);
    sub_082144A4(q, r4, 0);
    q->s = 0;
    r = Script_SeekToKeyword(0x70);
    if (r != 0) {
        q->x = Script_GetValue();
        q->y = Script_GetValue();
        r = Script_GetValue();
    } else {
        q->x = r;
        q->y = r;
    }
    q->z = r;
    r = Script_SeekToKeyword(0x66);
    if (r != 0) {
        r = Script_GetValue();
        if (r != 0) r = 4;
    }
    q->a = r;
}
