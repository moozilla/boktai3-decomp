#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Config{u16 v[20];};struct Vec{s16 x,y,z,pad;};extern u32 gUnk_03005308;extern u16 gUnk_0203B400[];extern s16 gUnk_086149C4[];void sub_080154EC(int,void*,struct Config*);
static inline int scale(int a,int b){int v=a*40;v*=b;if(v>=0)return v>>12;return -((-v)>>12);}
#define AnimatedParticles_EmitOffset sub_08015CF0
void AnimatedParticles_EmitOffset(struct Vec*pos,int unused,int type,int flags,int speed,int horizontal,int start,int decay){struct Config s;struct Vec out;s.v[0]=type;s.v[1]=flags;s.v[3]=speed;s.v[4]=0;s.v[5]=0;s.v[6]=horizontal;s.v[7]=0;gUnk_03005308=(gUnk_03005308+1)&1023;{u32 offset=gUnk_03005308*2;register u8*table asm("r2");table=(u8*)gUnk_0203B400;{u8*sample= (u8*)(offset+(u32)table);s.v[10]=*sample;}}s.v[11]=0;s.v[9]=0;s.v[8]=0;s.v[12]=start;s.v[13]=0;s.v[14]=decay;s.v[15]=0;s.v[16]=start;s.v[17]=0;s.v[18]=decay;s.v[19]=0;out.x=pos->x+scale(gUnk_086149C4[(s.v[10]+192)&255],horizontal);out.y=pos->y;out.z=pos->z+scale(gUnk_086149C4[(s.v[10]+128)&255],horizontal);sub_080154EC(1,&out,&s);}
