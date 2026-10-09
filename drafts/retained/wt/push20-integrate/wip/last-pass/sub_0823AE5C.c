#include "global.h"
struct BlobAE { u32 words[8]; };
struct BlobAE *sub_0821A520(u32, u32);
void sub_082196C4(struct BlobAE *, struct BlobAE *);
void sub_0821983C(u8 *, struct BlobAE *, u32, u32, u32, u32, u32, void *);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_08238238(u8 *);
void sub_08220D78(u8 *, struct BlobAE *, u32, u32, u32);
void sub_0823AE5C(u8 *p)
{
    struct BlobAE *data = sub_0821A520(0xcb05, 0xde23);
    if (data) {
        struct BlobAE *copy;
        u8 *object;
        s32 found;
        u32 zero;
        *(struct BlobAE *)(p + 0x6c) = *data;
        copy = (struct BlobAE *)(p + 0x6c);
        sub_082196C4(copy, data);
        object = p + 0x8c;
        sub_0821983C(object, copy, 0, 0x9000, 2, 0, 60, p + 0x30);
        found = Script_SeekToKeyword(0x64);
        if (found) p[0x548] = Script_GetValue();
        else p[0x548] = found;
        p[0x3a4] = p[0x548];
        zero = 0;
        sub_08238238(p);
        *(u16 *)(p + 0x3a0) = zero;
        sub_08220D78(object, copy, p[0x3a2], 1, zero);
        if (p[0x3a3]) *(u32 *)(p + 0x94) |= 4;
        else *(u32 *)(p + 0x94) &= ~4;
    }
}
