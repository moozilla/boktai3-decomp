#include "global.h"
extern u8 *gUnk_02000154;
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
u8 *sub_08126604(void);
u8 *sub_08126144(u8 *);
void sub_082151E4(u8 *,u32);
void *sub_0821A520(u32,u32);
void sub_082144A4(u8 *,u8 *,u32);
void sub_08220C8C(u8 *,void *,u32,u32,u32);
void sub_08215284(u8 *,u32);
s32 Mod(s32,s32);
void sub_0812642C(u32 *a,s32 b,s32 c,s32 d,s32 e,u32 f,s32 g,s32 h,u32 i,u32 j,u32 k,u32 l,u32 m,u32 n,u32 o,u32 p,u32 q,u32 r)
{
    struct Pair {u32 x,y;};
    u8 *context=gUnk_02000154;
    u8 *s,*resource,*fields;
    u32 zero;
    u16 *table;
    u32 *rng;
    u32 mask;
    s32 value;
    if(!context) {
        context=sub_08126604();
        if(!context) goto end;
    }
    fields=s=sub_08126144(context);
    if(!s) goto end;
    fields+=0x58;
    zero=0;
    *fields=1;
    fields+=0xE;
    *(u16 *)fields=zero;
    *(struct Pair *)(s+0x1C)=*(struct Pair *)a;
    resource=s+0x2C;
    sub_082151E4(resource,n);
    *(void **)(context+0x18)=sub_0821A520(0x922E,(u16)o);
    sub_082144A4(s,resource,0);
    sub_08220C8C(s+0x48,*(void **)(context+0x18),(u16)p,(u8)q,zero);
    sub_08215284(resource,r);
    table=gUnk_0203B400;
    rng=&gUnk_03005308;
    mask=0x3FF;
    *rng=(*rng+1)&mask;
    value=Mod(table[*rng],c)+b;
    *(u16 *)(s+0x5C)=value;
    *rng=(*rng+1)&mask;
    value=Mod(table[*rng],e)-(e>>1)+d;
    *(u16 *)(s+0x5E)=value;
    *rng=(*rng+1)&mask;
    value=Mod(table[*rng],h)+g;
    s[0x5A]=value;
    *(u16 *)(s+0x64)=f;
    *(u16 *)(s+0x68)=i;
    s[0x6D]=12;
    *(u32 *)s|=1|l;
    s[7]=j;
    *(u16 *)(s+0x6E)=m;
end:
    ;
}
