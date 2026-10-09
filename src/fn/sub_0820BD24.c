#include "global.h"
struct Args { s32 x, y, z, a, b, c, d, e; };
struct Message { u32 type : 16; u32 reserved : 16; struct Args *args; };
extern u8 *gUnk_02000710;
void sub_0821AD08(u32, struct Message *);
void sub_081D5204(s32);
void sub_08159164(s32);
#define H(p,n) (*(s16 *)((p)+(n)))
#define W(p,n) (*(u32 *)((p)+(n)))
static inline s32 clamp(s32 v) { if(v>9999) v=9999; else if(v<0) v=0; return v; }
#define INC(n) H(gUnk_02000710,n)=clamp(H(gUnk_02000710,n)+1)
void sub_0820BD24(u8 *p)
{
    struct Args a;
    struct Message m;
    u32 q = W(p,0x214);
    if (q) {
        a.x=H(p,0x40); a.y=H(p,0x42); a.z=H(p,0x44);
        a.a=W(p,0x208); a.b=W(p,0x20C); a.c=W(p,0x210);
        a.d=W(p,0x200); a.e=W(p,0x1F8);
        m.type=8; m.args=&a;
        sub_0821AD08(q,&m);
    }
    sub_081D5204(33);
    if (W(p,0x200)&1) {
        u16 flags;
        sub_08159164(H(p,0x1F0));
        H(gUnk_02000710,0x87C) = H(gUnk_02000710,0x87C)<9999 ? H(gUnk_02000710,0x87C)+1 : 9999;
        H(gUnk_02000710,0x538) = H(gUnk_02000710,0x538)<9999 ? H(gUnk_02000710,0x538)+1 : 9999;
        flags=H(p,0x1E8);
        H(gUnk_02000710,0x56A) = H(gUnk_02000710,0x56A)<9999 ? H(gUnk_02000710,0x56A)+1 : 9999;
        if (flags&0x80) { INC(0x58A); }
        else if (flags&0x100) { INC(0x58C); }
        else if (flags&0x200) { INC(0x58E); }
        else if (flags&0x400) { INC(0x590); }
        else if (flags&0x800) { INC(0x592); }
    }
}
