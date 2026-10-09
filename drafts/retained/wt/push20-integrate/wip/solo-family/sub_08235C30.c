#include "global.h"
struct Vec { s32 x:16,y:16,z:16; };
extern u16 gUnk_03005418[];
extern u32 gUnk_030053F4,gUnk_0300523C;
void sub_082359CC(u8 *);
void sub_08235AB8(u8 *);
void sub_08235B34(u8 *);
void sub_08235A5C(u8 *);
void sub_082358C0(u8 *);
static inline u8 *getOwner(u8 *p) { return *(u8 **)(p+0x18); }
static inline s32 scale(s32 v) { if(v>=0) v>>=5; else v=-((-v)>>5); return v; }
s32 sub_08235C30(u8 *p,u8 *owner,u32 mode)
{
    struct Vec v;
    u16 *dest;
    s16 *coords;
    s32 x,z,sum,vertical,y,depth;
    u32 *flags;
    u32 zero,mask;
    *(u8 **)(p+0x18)=owner;
    *(u16 *)(p+0x34c)=mode;
    v=*(struct Vec *)(getOwner(p)+0x30);
    v.y+=150;
    dest=(u16 *)(p+0x84);
    coords=(s16 *)&v;
    x=coords[0]; z=coords[2];
    dest[0]=scale((x-z)*3);
    sum=scale((x+z)*3);
    vertical=scale(coords[1]*3);
    y=sum-vertical;
    depth=sum+vertical;
    dest[0]=dest[0]-gUnk_03005418[0]+120;
    dest[1]=y-gUnk_03005418[1]+80;
    dest[2]=depth-gUnk_03005418[2];
    sub_082359CC(p);
    sub_08235AB8(p);
    sub_08235B34(p);
    owner=*(u8 **)(p+0x18);
    if(owner[0x419]==0) flags=(u32 *)(owner+0x94);
    else flags=(u32 *)(owner+0x14c);
    *flags&=~1;
    zero=0;
    *(u16 *)(*(u8 **)(p+0x18)+0x274)=zero;
    gUnk_030053F4|=1;
    mask=4;
    gUnk_0300523C|=mask;
    if(*(u16 *)(p+0x34c)==0) sub_08235A5C(p);
    *(void (**)(u8 *))(p+0x350)=sub_082358C0;
    *(u16 *)(p+0x34e)=zero;
    return 0;
}
