// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneActors_StartAnimation sub_080216E8
typedef struct {u8 pad[0xa3];u8 completed;u8 padA4[4];u8 facing;u8 padA9[13];u8 offset,mirror;u16 sequence;} Actor;
void sub_08220D78(void*,int,u16,u16,u16);
void SceneActors_StartAnimation(Actor*p,u8*animation,int resource,int sequence,int speed,int flags){p->offset=0;p->sequence=sequence;sub_08220D78(animation,resource,p->sequence,speed,flags);flags&=1;asm("" : "+r"(flags));if(!flags)p->completed=0;if(p->facing>1){p->mirror=1;*(u32*)(animation+8)|=4;}else{p->mirror=0;*(u32*)(animation+8)&=~4;}}
