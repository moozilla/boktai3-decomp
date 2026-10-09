#include "global.h"
s32 sub_0821A3E8(void *, void *);
void sub_0806FC24(u32, void *);
void sub_0806FC64(u32, void *);
void sub_0806FD08(u32, void *, u32);
struct Entry { u32 unk; u8 *data; };
void sub_0806FD5C(u8 *p)
{
    s32 n = sub_0821A3E8(*(void **)(p + 0x24), p + 0x18);
    if (n > 0) {
        struct Entry *q = *(struct Entry **)(p + 0x18);
        n--;
        do {
            u8 *d = q->data;
            switch (*(u16 *)d) {
            case 1: sub_0806FC24(d[2], d + 4); break;
            case 2: sub_0806FC64(d[2], d + 4); break;
            case 3: sub_0806FD08(d[2], d + 6, *(u16 *)(d + 4)); break;
            }
            q++;
        } while (n-- > 0);
    }
}
