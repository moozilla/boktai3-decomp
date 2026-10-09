#include "global.h"
struct Vec { s32 x:16,y:16,z:16; };
void sub_0821FEB4(u8 *,u32,u32,u32,u32,struct Vec *,struct Vec *);
void sub_0821FF58(u8 *,s32,s32,s32,s32,s32);
void sub_0821FF84(u8 *,u32,u8 *);
void sub_081DB030(u8 *owner,u8 *emitter,u32 flag,s32 duration,s32 arg4,s32 arg5,s32 arg6)
{
    struct Vec scale,offset;
    u32 type;
    scale.x=120; scale.y=120; scale.z=120;
    offset.x=0; offset.y=0; offset.z=0;
    type=0x2001;
    sub_0821FEB4(emitter,0,type,0,(u16)flag,&scale,&offset);
    sub_0821FF58(emitter,duration,arg4,528,arg5,arg6);
    sub_0821FF84(emitter,0,owner);
    if(duration==0) { if(arg4==0) *(u32 *)emitter|=4; }
}
