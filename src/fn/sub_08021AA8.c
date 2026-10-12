#include "global.h"
#define SceneActors_RunStateCallback sub_08021AA8
void SceneActors_RunStateCallback(u8*p){int tick;*(int*)(p+0x9c)=0;p[0xaf]=0;tick=(*(int*)(p+0x90))++;if(*(void**)(p+0x100))(*(void(**)(void*,int))(p+0x100))(p,tick);}
