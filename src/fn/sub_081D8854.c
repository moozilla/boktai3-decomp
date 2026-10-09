#include "global.h"
struct O {u32 flags;u8 pad[12];u16 frame;};
s32 sub_081D8854(u8 *p) {
 if(*(u32 *)p==3) {
 s32 d=(((u16)*(s8 *)(p+0x1e)+0x70)>>5);
 s32 mask=7;s32 *mask_pointer=&mask;
 struct O *o=(struct O *)(p+0x8c);
 d&=*mask_pointer;
 if(d&1)o->frame=0x20;else if((d>>1)&1)o->frame=0x21;else o->frame=0x1f;
 if(d<=2)o->flags&=~12;
 else if(d<=4)o->flags=(o->flags&~4)|8;
 else if(d<=5)o->flags|=12;
 else o->flags=(o->flags|4)&~8;
 }
 if(*(u32 *)p>5)return 2;return 0;
}
