#include "global.h"
extern u16 gUnk_0203B400[];
extern s32 gUnk_03005308;
void sub_0806A160(u8 *,u32,void *,u32,u32,u32,u32,u32);
void sub_0822B2F8(u32);
struct VAB {u32 x:16;u32 y:16;u32 z:16;};
static inline u16 rnd(u16 *tbl, s32 *idx)
{
 *idx=(*idx+1)&0x3ff;
 return tbl[*idx];
}
void sub_080AB294(u8 *p)
{
 struct VAB v;
 u16 *tbl;
 s32 *idx;
 v.x=0x50;
 v.y=0x3c;
 v.z=0x3c;
 tbl=gUnk_0203B400;
 idx=&gUnk_03005308;
 sub_0806A160(p,(rnd(tbl,idx)&3)+1,&v,0x20,4,20,2,1);
 sub_0806A160(p,(rnd(tbl,idx)&1)+1,&v,0x40,4,20,2,1);
 sub_0806A160(p,(rnd(tbl,idx)&3)+1,&v,0x5e,4,20,2,1);
 sub_0806A160(p,(rnd(tbl,idx)&1)+1,&v,0x80,4,20,2,1);
 sub_0806A160(p,(rnd(tbl,idx)&3)+1,&v,0xc4,4,20,2,1);
 sub_0806A160(p,(rnd(tbl,idx)&1)+1,&v,0xf0,4,20,2,1);
 sub_0822B2F8(0xe5);
}
