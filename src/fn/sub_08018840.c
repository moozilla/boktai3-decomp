#include "global.h"
struct Orb{u8 state,angle;u16 rise,radius,timer;u32 flags;u8 tail[36];};struct Manager{u8 pad[32];u16 speed,limit,count,frame;void*resource;};void sub_08217EEC(void*,void*,int);
#define OrbitingParticles_Advance sub_08018840
void OrbitingParticles_Advance(struct Manager*m,struct Orb*p){p->rise+=m->speed;if(!(p->timer&3))sub_08217EEC(&p->flags,m->resource,m->frame+((p->timer>>2)&1));p->timer++;if(p->rise>=m->limit){p->state=0;p->flags|=1;p->timer=((8-m->count)<<2)+4;}}
