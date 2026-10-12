#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Clock{u8 pad[24];s16 hour,minute,second,tick;int seconds,window;void*script;u8 armed[4];};int sub_08228D88(void);int sub_08228D94(void);int sub_08228DA0(void);int sub_0821ABA8(int,int);
#define ClockEvents_Initialize sub_08019474
int ClockEvents_Initialize(struct Clock*p){int i;u8*q;p->hour=sub_08228D88();p->minute=sub_08228D94();p->second=sub_08228DA0();p->seconds=p->hour*3600+p->minute*60+p->second;p->window=sub_0821ABA8('d',60);p->script=(void*)sub_0821ABA8('p',0);{register int one asm("r2")=1;register int count asm("r1")=3;asm("" : "+r"(one), "+r"(count));q=&p->armed[3];do{*q=one;q--;count--;}while(count>=0);}p->tick=0;return 0;}
