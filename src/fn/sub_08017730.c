#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec{s16 x,y,z,pad;};struct Sprite{u8 pad[24];struct Vec position;u8 tail[8];};struct Drop{u8 active,timer,state,bank;struct Vec positions[2];struct Sprite sprite;};struct Weather{u8 pad[360];void*resource;struct Drop drops[32];};
void sub_08217EEC(void*,void*,int);void sub_08217EAC(void*);
#define WeatherParticles_Update sub_08017730
int WeatherParticles_Update(struct Weather*p){struct Drop*d=p->drops;int i;for(i=31;i>=0;i--,d++){struct Sprite*s=&d->sprite;asm("" : "+r"(s));if(d->active){d->sprite.position=*(struct Vec*)((d->bank*8)+(u32)d+4);d->bank=1-d->bank;switch(d->state){case 0:if(d->timer>2){d->state=1;sub_08217EEC(s,p->resource,31);d->timer=0;}break;case 1:if(d->timer>2){sub_08217EAC(s);d->active=0;}break;}d->timer++;}}}
