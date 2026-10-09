#include "global.h"
struct Pos {s16 x,y,z,pad;};
struct Pos *sub_0815F53C(u32);
void sub_08220D20(void*,void*,u32,u32,u32);void sub_0822B2F8(u32);
void sub_0820DEE0(void);
u32 sub_0820DE60(u8 *p)
{
 struct Pos *pos=sub_0815F53C(0);
 struct Pos *origin=(struct Pos*)(p+0x40);
 s32 x=pos->x-origin->x;
 s32 z=pos->z-origin->z;
 s32 xx=x*x,zz=z*z;
 if(xx+zz < *(s32*)(p+0x11c)) {
  *(void**)(p+0x108)=sub_0820DEE0;
  sub_08220D20(p+0x10c,*(void**)(p+0x168),24,0,8);
  *(u16*)(p+0x128)=180;
  sub_0822B2F8(0x51c);
 }
 return 0;
}
