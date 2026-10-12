#include "global.h"
struct Sprite{u8 pad[16];u16 frame;};
struct Indicator{u8 pad0[8];u32 timer;u8 padc[5];s8 frame;u8 pad12,index,count,visible,mode,disabled;int delay,period,value; s16 times[8];u8 effect[84];struct Sprite sprite;};
void sub_0822B3BC(int);void sub_0821FE40(void*);
static inline void setFrame(struct Sprite*s,int frame){s->frame=frame;}
#define SceneIndicator_Update sub_08011928
void SceneIndicator_Update(u8*owner,struct Indicator*p){u8 old;if(p->disabled){if(p->visible){p->visible=0;setFrame(&p->sprite,p->frame);}return;}old=p->visible;switch(p->mode){case 0:if(p->timer>=(u32)p->times[p->index]){p->index++;if(p->index>=p->count)p->index=0;p->visible=1-(p->index&1);if(p->visible)sub_0822B3BC(0x5AA);p->timer=0;}break;case 1:if(!old){if(!p->delay){if(p->value==*(int*)(owner+0xBCC))p->delay=1;}else{p->delay++;if(p->delay>=p->period){p->visible=1;p->delay=60;sub_0822B3BC(0x5AA);}}}else{if(!p->delay)p->visible=0;else p->delay--;}break;case 2:break;}{int visible=p->visible;if(old!=visible)setFrame(&p->sprite,p->frame+visible);}if(p->visible==1)sub_0821FE40(p->effect);p->timer++;}
