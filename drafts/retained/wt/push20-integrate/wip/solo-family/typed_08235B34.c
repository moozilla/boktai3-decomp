#include "global.h"
struct Vec { s32 x:16,y:16,z:16; };
extern u8 *gUnk_02000710;
void sub_0821FEB4(u8 *,u32,u32,u32,u32,struct Vec *,struct Vec *);
void sub_0821FF58(u8 *,s32,s32,s32,s32,s32);
void sub_0821FF84(u8 *,u32,u8 *);
void sub_0821FF24(u8 *,u8 *,u32);
static inline void setup(u8 *e,u32 flag,struct Vec *scale,struct Vec *offset) { sub_0821FEB4(e,0,0x3001,0,(u16)flag,scale,offset); }
struct Obj { u8 pad0[0x18]; u8 *owner; u8 pad1C[0x330]; u16 mode; };
void sub_08235B34(struct Obj *p)
{
    u8 *emitter=(u8 *)p+0x94;
    struct Vec scale,offset;
    s32 duration;
    u32 flag;
    u32 speed;
    scale.x=30000; scale.y=30000; scale.z=30000;
    offset.x=0; offset.y=0; offset.z=0;
    if(p->mode==0) {
        u8 *owner=p->owner;
        flag=1<<owner[0x2c];
        duration=*(u16 *)(owner+0x426)-*(u16 *)(owner+0x424);
    } else {
        s32 value;
        if((p->owner)[0x418]==6) {
            value=*(s16 *)(gUnk_02000710+0x40)+10;
            if(value>99) value=99;
        } else value=*(s16 *)(gUnk_02000710+0x40);
        flag=0;
        duration=value*8;
    }
    speed=4096;
    setup(emitter,flag,&scale,&offset);
    sub_0821FF58(emitter,duration,10,0,speed,30);
    sub_0821FF84(emitter,0,(u8 *)p);
    sub_0821FF24(emitter,p->owner+0x30,0);
}
