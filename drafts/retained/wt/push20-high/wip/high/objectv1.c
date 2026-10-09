#include "global.h"
struct Pair {u32 a,b;};
struct Pos {u16 x,y,z,pad;};
extern u8 *gUnk_020005F8;
u8 *sub_0820112C(void*,s32*);void sub_08219EBC(void*,void*,u32);
void sub_0821980C(void*,void*,u32,u32);
u8 *sub_0815F53C(u32);
void sub_08201270(void*,void*);void sub_082003BC(void);void sub_082010EC(void);
s32 sub_08201160(u8 *resource,struct Pos *pos)
{
 u8 *pool=gUnk_020005F8;
 if(pool) {
  s32 index;
  u8 *obj=sub_0820112C(pool,&index);
  if(obj) {
   void *handler=obj+0x198;
   void *display;
   u32 zero;
   u8 *tmp;
   sub_08219EBC(handler,resource,0x20);
   zero=0;*(u32*)obj=zero;*(u32*)(obj+0x308)=zero;*(u16*)(obj+0x330)=zero;*(u32*)(obj+0x31c)=zero;
   display=obj+0x1b8;
   sub_0821980C(display,obj+0x278,0,1);
   *(u32*)(obj+0x324)=*(u32*)(resource+8);obj[0x1d2]=2;
   *(struct Pair*)(obj+0x1d8)=*(struct Pair*)pos;
   *(struct Pair*)(obj+0x1c)=*(struct Pair*)pos;
   *(u16*)(obj+0x14)=pos->x;*(u16*)(obj+0x16)=pos->y+0x80;*(u16*)(obj+0x18)=pos->z;
   tmp=sub_0815F53C(0);*(struct Pair*)(obj+0x24)=*(struct Pair*)tmp;
   CpuSet(&zero,obj+0xc,0x05000002);
   *(u32*)((u8*)display+8)|=1;
   sub_08201270(handler,sub_082003BC);
   *(u32*)(pool+0x18)|=1<<index;
   (*(u32*)(pool+0x9e8))++;
   return index;
  }
 } else sub_082010EC();
 return -1;
}
