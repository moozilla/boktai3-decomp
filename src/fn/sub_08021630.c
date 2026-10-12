#include "global.h"
#define SceneActors_SetHealth sub_08021630
int Div(int,int);
int SceneActors_SetHealth(u8*p,int health){*(u16*)(p+0x94)=health;if(*(s16*)(p+0x94)>*(s16*)(p+0x96))*(u16*)(p+0x94)=*(u16*)(p+0x96);else if(*(s16*)(p+0x94)<0)*(u16*)(p+0x94)=0;if(*(s16*)(p+0x96))p[0xe1]=Div(*(s16*)(p+0x94)<<7,*(s16*)(p+0x96));else p[0xe1]=0;if(p[0xe1]<=63)p[0xa0]=1;else p[0xa0]=0;return 0;}
