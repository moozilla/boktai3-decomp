#include "global.h"
struct Vec{s32 x:16,y:16,z:16,pad:16;};
void sub_082144A4(void*,void*,int);void sub_0821FEB4(void*,int,int,int,int,void*,void*);void sub_0821FF84(void*,void*,void*);void sub_0821FF58(void*,int,int,int,int,int);
#define SceneIndicator_Initialize sub_08011890
void SceneIndicator_Initialize(u8*m,u8*p){struct Vec size,offset;int zero;u8*sprite=p+0x88,*effect;sub_082144A4(sprite,m+0x1c,1);zero=0;sprite[7]=3;effect=p+0x34;size.x=120;size.y=32;size.z=120;offset.x=0;offset.y=32;offset.z=0;sub_0821FEB4(effect,0,0x2001,0,16,&size,&offset);sub_0821FF84(effect,0,p);sub_0821FF58(effect,10,1,0x400000,0x60000,30);p[16]=zero;}
