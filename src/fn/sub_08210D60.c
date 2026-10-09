#include "global.h"
struct Tile { u8 p0[0x18];u16 h18;u8 p1A[6];u16 h20,h22; };
extern struct Tile gUnk_03004CC0;
extern u16 gUnk_03004FC0[];
void sub_08215A74(u32,u32);
void sub_082156B8(u32);void sub_0821656C(u32,u32,u32,u32,u32);
u8 *sub_0821A520(u32,u32);
void sub_082161B4(u32,u32,void*,u32,u32,u32,u32*);
s32 sub_08210A1C(s32);
void sub_08215EB4(s32,s32,s32,s32,s32,s32,s32,s32);
void sub_08210D60(u8 *p)
{
 u32 size;
 s32 max,count,row;
 struct Tile *tile;
 sub_082156B8(0);sub_0821656C(0,0,0,0,0);
 sub_082156B8(2);sub_0821656C(2,0,0,0,0);
 sub_082156B8(3);sub_0821656C(3,0,0,0,0);
 {
  u8 *res=sub_0821A520(0xc091,0x3536);
  *(u8**)(p+0x18)=res;
  size=16;sub_082161B4(2,0,res,0,0,1,&size);
 }
 sub_08215A74(3,3);
 sub_0821656C(3,0,0,0,0);
 CpuSet(sub_0821A520(0x92b3,0x204)+0x14,gUnk_03004FC0,0x100);
 count=0;
 max=sub_08210A1C(*(s16*)(p+0xb80));
 row=0;tile=&gUnk_03004CC0;
 do {
  int next=row+1;
  int x=3,y=row*5,i=4;
  do {
   tile->h18=16;tile->h20=0;tile->h22=0;
   count++;
   if(count>max) sub_08215EB4(3,x,y+3,4,4,1,0,2);
   x+=5;i--;
  } while(i>=0);
  row=next;
 } while(row<=1);
}
