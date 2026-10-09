#include "global.h"
struct Vec { u32 x:16; s32 y:16; u32 z:16,pad:16; };
void sub_0811FAB8(void*),sub_0811FCB4(void*);
void sub_0811B278(void*,struct Vec*,struct Vec*,struct Vec*,u32,u32,u32,u32,u32,void(*)(void*),void*);
void sub_0811B2FC(void*);
void sub_0811FDB4(u8 *obj) {
 struct Vec p,v,w;
 u16 *x=(u16*)(obj+0x618),*y=(u16*)(obj+0x61a);
 s32 value;
 p.x=*x;value=*y-96;p.y=value;p.z=value;
 v.x=24;v.y=32;v.z=2;
 w.x=0;w.y=-v.y;w.z=-v.z;
 sub_0811B278(obj+0x56c,&p,&v,&w,20,5,1,0,12,sub_0811FAB8,obj);
 p.x=*x;value=*y-96;p.y=value;p.z=value;
 v.x=24;v.y=32;v.z=2;
 w.x=0;w.y=-v.y;w.z=-v.z;
 sub_0811B278(obj+0x52c,&p,&v,&w,12,5,1,0,12,sub_0811FCB4,obj);
 sub_0811B2FC(obj+0x56c);
}
