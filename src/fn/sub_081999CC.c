#include "global.h"
struct V {s16 x,y,z;};struct P {u32 x:16,y:16,z:16,w:16;};
extern u8 *gUnk_02000218,*gUnk_02000710;extern u32 gUnk_0300523C;extern struct V gUnk_03005418;
static inline s32 sh(s32 v){s32 r;if(v>=0)r=v>>5;else r=-((-v)>>5);return r;}
static inline void project(struct V *out,struct V *in){s32 a,b,c,d,x=in->x,z=in->z;out->x=sh(3*(x-z));a=sh(3*(x+z));b=sh(3*in->y);c=a-b;d=a+b;out->x=out->x-gUnk_03005418.x+120;out->y=c-gUnk_03005418.y+80;out->z=d-gUnk_03005418.z;}
s32 sub_081999CC(void){
 struct P in;struct V out;u8 *g,*actor; s32 next,left,stride;
 if(!gUnk_02000218 || (gUnk_0300523C&7))goto fail;
 actor=gUnk_02000710;in.x=*(u16 *)(actor+0x30);in.y=*(u16 *)(actor+0x32);in.z=*(u16 *)(actor+0x34);
 project(&out,(struct V *)&in);
 {s32 position=out.x;left=1;if(position>120)left=0;}
 g=gUnk_02000218;next=g[0x1e];next++;if(next>1)next-=2;
 if(next!=g[0x1f])goto write;
fail:return 0;
write:
 {u8 *base=g;u8 **bp=&base;
 *(*bp+g[0x1e]*(stride=4)+0x2c)=2;*(*bp+g[0x1e]*stride+0x2d)=left;*(*bp+g[0x1e]*stride+0x2e)=1;g[0x1e]=next;return 1;}
}
