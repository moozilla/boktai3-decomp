#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec{s16 x,y,z,pad;};
struct Obj{u32 flags;u8 pad[12];u16 frame;u8 pad12[10];struct Vec position;u8 tail[8];};
struct Anim{const u16*data;u8 flags,count,factor,rate;u16 frame,unused,scale,timer;};
struct Piece{u8 active,pad1;u16 timer;s16 verticalScale,horizontalScale;u16 verticalStart,verticalDecay,horizontalStart,horizontalDecay;struct Vec direction,velocity;u8 resource[28];struct Obj sprite;struct Anim anim;};
struct Manager{u8 pad[24];void*resources[2];u32 timer;struct Piece pieces[32];};
void sub_08214514(void*);
extern struct Manager*gUnk_02000068;extern u16 gUnk_08604C54[];
void*sub_0821A520(int,int);int sub_082151E4(void*,int);void sub_082144E4(void*,void*,int);void sub_08220C8C(void*,void*,int,int,int);
#define AnimatedParticleManager_Initialize sub_080153B0
int AnimatedParticleManager_Initialize(struct Manager*m){int i;register int clear asm("r9");struct Vec zero;struct Piece*p;struct Obj*o;struct Anim*a;u8*r;
gUnk_02000068=m;for(i=0;i<=1;i++)m->resources[i]=sub_0821A520(0x922e,gUnk_08604C54[i]);
zero.x=0;zero.y=0;zero.z=0;
i=0;clear=0;asm("" : "+r"(clear));a=&m->pieces[0].anim;o=&m->pieces[0].sprite;r=m->pieces[0].resource;p=m->pieces;
for(;i<=31;i++){p->active=clear;p->verticalScale=clear;p->direction.x=clear;p->direction.y=clear;p->direction.z=clear;
if(!sub_082151E4(r,0x4384))return -1;
sub_082144E4(o,r,0);sub_08220C8C(a,m->resources[0],0,0,clear);
*((u8*)o+7)=2;*(struct Vec*)((u8*)p+88)=zero;
a=(void*)((u8*)a+120);o=(void*)((u8*)o+120);r+=120;p++;
}m->timer=0;return 0;}
