#include "global.h"
struct Data {u8 pad0[0x14];u32 w14,w18;s16 h1c,h1e,h20,h22;u8 pad24[0x10];u32 w34;};
void sub_08202E4C(void*);
void sub_082151E4(void*,u32);void sub_082144E4(void*,void*,u32);
void sub_0821FEB4(void*,u32,u32,u32,u32,void*,void*);
void sub_0821FF84(void*,void*,void*);
void sub_0821FF58(void*,s32,s32,u32,u32,s32);
void sub_0820264C(void);
void sub_0820267C(void *unused,u8 *p)
{
 struct Data *data=(struct Data*)(p+0x28);
 void *obj,*effect,*target;
 u32 zero;
 sub_08202E4C(data);*(u32*)p=0;
 target=p+0x10;zero=0;
 {CpuSet(&zero,target,0x05000002);}
 obj=p+0x84;
 sub_082151E4(obj,data->w34);
 sub_082144E4(p+0xa0,obj,1);
 effect=p+0xcc;
 sub_0821FEB4(effect,0,0x2001,0,16,p+0x4c,p+0x54);
 sub_0821FF84(effect,sub_0820264C,p);
 sub_0821FF58(effect,data->h1c,data->h1e,data->w14,data->w18,data->h22);
}
