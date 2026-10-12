#include "global.h"
struct Vec{s16 x,y,z,pad;};struct Sprite{u32 flags;u8 pad[12];u16 frame;u8 pad12[10];struct Vec position;u8 tail[8];};struct Hazard{u8 active,state,entered,pad3;u16 duration,pad6;u32 timer;u8 padc[88];struct Sprite sprite;};
extern u8*gUnk_02000710;struct Vec*sub_0815F53C(int);void sub_0801204C(void*,void*,int);
#define AnimatedHazard_WaitForPlayer sub_08012138
void AnimatedHazard_WaitForPlayer(void*m,struct Hazard*p){if(*(s16*)(gUnk_02000710+0x876)>0)p->sprite.flags&=~1;else p->sprite.flags|=1;if(++p->timer>p->duration)sub_0801204C(m,p,3);else{struct Vec*player=sub_0815F53C(0);struct Vec*pos=&p->sprite.position;int x=player->x-pos->x,z=player->z-pos->z;if(x*x+z*z<14400)sub_0801204C(m,p,3);}}
