#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Clock{u8 pad[24];s16 hour,minute,second,tick;int seconds,window;void*script;u8 armed[4];};extern u8 gUnk_03005430[];int sub_08228D88(void);int sub_08228D94(void);int sub_08228DA0(void);void sub_080192D0(void*,int);
static inline int distance(int a,int b){int d=a-b;if(a<b)d=b-a;if(d>43199)d-=86400;{register int v asm("r0")=d;asm("" : "+r"(v));if(v<0)v=-v;return v;}}
static inline int scheduled(int index){int h=gUnk_03005430[index]*3600;asm("" : "+r"(h));return h+gUnk_03005430[index+1]*60;}
#define ClockEvents_Update sub_080192FC
int ClockEvents_Update(struct Clock*p){register int zero asm("r5");int tick=(u16)p->tick+1;zero=0;asm("" : "+r"(zero));p->tick=tick;if(p->tick>59)p->tick=zero;else{p->hour=sub_08228D88();p->minute=sub_08228D94();p->second=sub_08228DA0();p->seconds=p->hour*3600+p->minute*60+p->second;if(distance(p->seconds,43230)<=30){if(p->armed[0]){p->armed[0]=zero;sub_080192D0(p,0);}}else p->armed[0]=1;{int when=scheduled(10),limit=p->window;asm("" : "+r"(limit));if(distance(p->seconds,when)<=limit){if(p->armed[1]){p->armed[1]=0;sub_080192D0(p,1);}}else p->armed[1]=1;}{int when=scheduled(12),limit=p->window;asm("" : "+r"(limit));if(distance(p->seconds,when)<=limit){if(p->armed[2]){p->armed[2]=0;sub_080192D0(p,2);}}else p->armed[2]=1;}}return 0;}
