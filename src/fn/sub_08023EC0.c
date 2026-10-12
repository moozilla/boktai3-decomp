#include "global.h"
typedef struct State {u8 pad0[0x90];u32 tick;s16 hp,maxHp;u16 extra;u8 vulnerable,kind;u32 flags;u8 unknownA0[2];u8 unknownA2,unknownA3;u32 unknownA4;u8 facing,facing8,animation,state,mode,submode,phase,finished,enter,changed;u8 padB2[0x12];u32 elapsed;u8 padC8[0x38];void(*callback)(void*,int);} State;
typedef struct Actor {State base;u8 pad104[0x23f];u8 variant;u8 pad344[0x12];u16 delay;} Actor;
static inline u8 enter(State*p){if(p->enter){p->enter=0;p->finished=0;return 1;}return 0;}
void sub_080224A4(void*,int);void sub_08022DBC(void*);
void sub_08023EC0(Actor*p){State*s=&p->base;s->vulnerable=1;if(enter(s))sub_080224A4(p,4);if(s->finished){if(p->base.state!=3){sub_08022DBC(p);p->delay=60;return;}else {u32 k=4;p->base.animation=k;}}s->elapsed++;}
