#include "global.h"
// MATCHING_CONCESSION: register-pinning
extern void*gUnk_0200006C;void*sub_08215184(int);void*sub_0821A520(int,int);void sub_08217E60(void*,void*,int);void sub_08220C8C(void*,void*,int,int,int);
#define FadingParticleManager_Initialize sub_080163C8
int FadingParticleManager_Initialize(u8*m){int i;register int clear asm("r9");gUnk_0200006C=m;*(void**)(m+28)=sub_08215184(0x1c1a);*(void**)(m+32)=sub_0821A520(0x922e,0xd1b8);i=0;clear=0;asm("" : "+r"(clear));do{u8*g=m+36+i*548;u8*a,*s;int j,next;g[0]=clear;next=i+1;a=g+56;s=g+16;for(j=7;j>=0;a+=68,s+=68,j--){sub_08217E60(s,*(void**)(m+28),0);sub_08220C8C(a,*(void**)(m+32),3,0,clear);}i=next;}while(i<=5);*(u32*)(m+24)=0;return 0;}
