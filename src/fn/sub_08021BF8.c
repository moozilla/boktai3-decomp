#include "global.h"
#define SceneActors_ConfigureEffect sub_08021BF8
void*sub_08066DB8(void*,void*,int,int,void*,int,int,int);void sub_0822B2F8(int);
void SceneActors_ConfigureEffect(u8*p,unsigned mode,int first,int second){switch(mode){
case 0:*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,0,1,0,0,0,100);sub_0822B2F8(0x1b1);break;
case 2:*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,0,0,0,0,0,100);sub_0822B2F8(0x1ec);break;
case 1:*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,0,3,0,0,0,100);sub_0822B2F8(0x1ec);break;
case 3:*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,0,2,0,0,0,100);sub_0822B2F8(0x1ed);break;
case 4:{u16 *out=(u16*)(p+0x150);int zero=0;*out=first;*(u16*)(p+0x152)=second;*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,1,zero,out,*(s16*)(p+0x152),zero,100);break;}
case 5:{u16 *out=(u16*)(p+0x150);int zero=0;*out=first;*(u16*)(p+0x152)=second;*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,3,zero,out,*(s16*)(p+0x152),zero,100);break;}
case 6:*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,5,0,0,0,0,100);break;
case 7:*(void**)(p+0x14c)=sub_08066DB8(*(void**)(p+0x14c),p+0x48,4,0,0,0,0,100);break;
case 8:default:break;}}
