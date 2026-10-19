#include "global.h"

struct Hdr {
    u32 w[8];
};

struct Slot {
    u32 a;
    u8 b;
    u8 pad5;
    u8 c;
    u8 pad7[5];
    u32 d;
    u8 pad10[0x84 - 0x10];
};

struct Obj {
    u8 pad0[0x18];
    u32 f18;
    struct Hdr hdr;
    struct Slot slots[8];
};

extern struct Obj *gUnk_02000078;
void CpuSet(const void *src, void *dest, u32 control);
s32 ResourceHeader_Relocate(void *dst, void *src);
void *ScriptProc_Start(u16 key, u16 context);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);

s32 sub_08016F24(struct Obj *p)
{
    struct Hdr *src;
    u32 zero;
    u32 buf[2];
    u8 *s;
    u8 *h;
    s32 i;

    gUnk_02000078 = p;
    src = ScriptProc_Start(0xCB05, 0x5D04);
    if (src == 0)
        return -1;
    p->hdr = *src;
    ResourceHeader_Relocate(&p->hdr, src);
    p->f18 = 0;
    zero = 0;
    CpuSet(&zero, buf, 0x05000002);
    {
        void *hp = &p->hdr;
        u32 z = 0;
        u32 one = 1;
        h = (u8 *)p + 0x60;
        s = (u8 *)p + 0x3c;
        i = 7;
        do {
            *(u32 *)s = z;
            s[4] = one;
            s[6] = z;
            *(u32 *)(s + 0xc) = z;
            sub_0821983C(h, hp, 0, 0x11, one, z, 0x3c, buf);
            h += 0x84;
            s += 0x84;
            i--;
        } while (i >= 0);
    }
    return 0;
}
