#include "global.h"
struct Elem { u32 flags; u8 pad[40]; };
extern u32 gUnk_0300523C;
void sub_08235328(u8 *);
void sub_08235574(u8 *);
void sub_082352C8(u8 *);
static inline void clear(u32 *dst,u32 mask) { *dst&=mask; }
#define H(o) (*(u16 *)(p+(o)))
void sub_0823565C(u8 *p)
{
    u32 age=H(0x34e);
    if(age==0 || age==18 || age==36 || age==54 || age==72 || age==90) sub_08235328(p);
    sub_08235574(p);
    H(0x34e)++;
    if(H(0x34e)>121) {
        u32 *flags=&gUnk_0300523C;
        void (*next)(u8 *)=sub_082352C8;
        u32 one=1;
        s32 addr=(s32)(p+0xe8),end=(s32)(p+0x114);
        u32 zero;
        do { *(u32 *)addr|=one; addr+=44; } while(addr<=end);
        zero=0;
        clear(flags,~4);
        *(void (**)(u8 *))(p+0x350)=next;
        H(0x34e)=zero;
    }
}
