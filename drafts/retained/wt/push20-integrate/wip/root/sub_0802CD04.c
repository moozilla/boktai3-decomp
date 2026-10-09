#include "global.h"
s32 sub_0821ABA8(s32,s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0802C8AC(u8 *,s32,s32,void *);
void sub_0802C91C(u8 *,s32,s32);
struct Fields2CD {u8 pad[0x13c];u8 v13c;u8 pad2[3];u32 v140;u8 v144,v145;};
struct Vec2CD {u32 x:16;u32 y:16;u32 z:16;};
void sub_0802CD04(u8 *p)
{
 struct Vec2CD v;
 s32 id=sub_0821ABA8(0x6e,0);
 s32 n;
 if(Script_SeekToKeyword(0x70)) {
  v.x=Script_GetValue();
  v.y=Script_GetValue();
  v.z=Script_GetValue();
 } else {
  v.x=0;
  v.y=0;
  v.z=0;
 }
 sub_0802C8AC(p,id,sub_0821ABA8(0x64,0),&v);
 *(u32 *)(p+0x50)=0;
 *(u32 *)(p+0x54)=0;
 *(u32 *)(p+0x58)=0;
 *(u32 *)(p+0x5c)=0;
 *(u32 *)(p+0x60)=0;
 *(u32 *)(p+0x138)=sub_0821ABA8(0x65,0x10000);
 {struct Fields2CD *q=(struct Fields2CD *)p;
 q->v13c=sub_0821ABA8(0x45,0x80);
 q->v140=-1;
 q->v144=0x7f;
 q->v145=0xff;}
 p[0x146]=sub_0821ABA8(0x73,1);
 n=sub_0821ABA8(0x6c,0x64);
 sub_0802C91C(p,n,n);
 if(Script_SeekToKeyword(0x53)) {
  *(u32 *)(p+0x14c)=Script_GetValue();
  *(u32 *)(p+0x150)=Script_GetValue();
  *(u32 *)(p+0x154)=Script_GetValue();
  *(u32 *)(p+0x158)=Script_GetValue();
  *(u32 *)(p+0x15c)=Script_GetValue();
  *(u32 *)(p+0x160)=Script_GetValue();
  *(u32 *)(p+0x164)=Script_GetValue();
  *(u32 *)(p+0x168)=Script_GetValue();
 } else {
  *(u32 *)(p+0x14c)=10;
  *(u32 *)(p+0x150)=50;
  *(u32 *)(p+0x154)=20;
  *(u32 *)(p+0x158)=10;
  *(u32 *)(p+0x15c)=0;
  *(u32 *)(p+0x160)=0;
  *(u32 *)(p+0x164)=0;
  *(u32 *)(p+0x168)=0;
 }
 *(u32 *)(p+0x16c)=sub_0821ABA8(0x44,0);
 if(Script_SeekToKeyword(0x61)) {
  u32 *q=(u32 *)(p+0x170);
  s32 i=3;
  do {*q++=Script_GetValue();} while(--i>=0);
 }
}
