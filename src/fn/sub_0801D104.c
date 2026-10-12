#include "global.h"
#define ResultTutorial_FadeIn sub_0801D104
void sub_08219574(int,int,int);
int ResultTutorial_FadeIn(u8*p){int result;if(p[0xb5]!=16){int v=(*(s16*)(p+0xaa)>>1);sub_08219574(0,8-v,v+8);p[0xb5]=v+8;++*(u16*)(p+0xaa);result=0;}else result=1;return result;}
