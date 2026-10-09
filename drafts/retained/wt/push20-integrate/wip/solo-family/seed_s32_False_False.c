#include "global.h"
struct Effect { u32 flags; u8 pad4[0x14]; u16 x,y,z; u8 pad1E[10]; u16 age,active,dx,dy,dz; u16 pad32; void *data; void (*update)(struct Effect *); };
void sub_08217EEC(struct Effect *,void *,u32);
#define H(o) (*(u16 *)(p+(o)))
#define W(o) (*(u32 *)(p+(o)))
void sub_08234810(u8 *p)
{
    s32 age=H(0x28)+1;
    H(0x28)=age;
    if(H(0x28)>15) { W(0)|=1; H(0x2a)=0; }
    else {
        u32 dx,x,dy,y;
        sub_08217EEC((struct Effect *)p,*(void **)(p+0x34),((H(0x28)>>2)&1)+2);
        dx=H(0x2c); x=H(0x18); H(0x18)=dx+x;
        dy=H(0x2e); y=H(0x1a); H(0x1a)=dy+y;
        if(!(H(0x28)&7)) H(0x2e)=dy+1;
    }
}
