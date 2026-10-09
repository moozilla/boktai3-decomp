#include "global.h"

s32 sub_0821A3E8(void *, void *);
void sub_0806FC24(u32, void *);
void sub_0806FC64(u32, void *);
void sub_0806FD08(u32, void *, u32);

void sub_0806FD5C(u8 *a) {
    s32 count = sub_0821A3E8(*(u32 *)(a + 0x24), a + 0x18);
    if (count > 0) {
        u8 *entry = *(u8 **)(a + 0x18);
        count--;
        do {
            u8 *p = *(u8 **)(entry + 4);
            switch (*(u16 *)p) {
            case 1: sub_0806FC24(p[2], p + 4); break;
            case 2: sub_0806FC64(p[2], p + 4); break;
            case 3: sub_0806FD08(p[2], p + 6, *(u16 *)(p + 4)); break;
            }
            entry += 8;
            {
                s32 prev = count;
                count--;
                if (prev <= 0) break;
            }
        } while (1);
    }
}
