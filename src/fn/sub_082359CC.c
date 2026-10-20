#include "global.h"
struct Blob59 {
    u32 words[8];
};
struct Blob59 *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(struct Blob59 *, struct Blob59 *);
void sub_0821983C(u8 *, struct Blob59 *, u32, u32, u32, u32, u32, void *);
void sub_082359CC(u8 *p)
{
    struct Blob59 *data = ScriptProc_Start(0xcb05, 0x5d04);
    struct Blob59 copy = *data;
    u32 kind, zero, x, value;
    u16 *dest, *src, *q, v;
    ResourceHeader_Relocate(&copy, data);
    value = *(u16 *)(p + 0x34c);
    kind = 48;
    if (value == 0)
        kind = 47;
    src = (u16 *)(p + 0x84);
    x = *src - 8;
    dest = (u16 *)(p + 0x7c);
    zero = 0;
    *dest = x;
    v = *(u16 *)(p + 0x86) - 40;
    dest++;
    *dest = v;
    v = *(u16 *)(p + 0x88);
    q = (u16 *)(p + 0x80);
    *q = v;
    sub_0821983C((u8 *)q - 0x64, &copy, kind + 104, 50, 1, zero, 60, src);
    p[0x51] = 34;
    p[0x50] = 34;
}
