#include "global.h"
extern void *gUnk_02000060;void *sub_08215184(int);void sub_08217E60(void*,void*,int);void sub_08217EC4(void*,int,int);
#define ParticleManager_Initialize sub_08014568
int ParticleManager_Initialize(u8*m){int i;gUnk_02000060=m;*(void**)(m+28)=sub_08215184(0x1c1c);*(void**)(m+32)=sub_08215184(0x1c1e);for(i=0;i<=5;){int offset=i*452;u8*g=m+offset;u8*s;int j,next;g+=36;g[0]=0;next=i+1;s=m+(offset+56);for(j=7;j>=0;s+=56,j--){sub_08217E60(s,*(void**)(m+28),1);sub_08217EC4(s,-4,-4);}i=next;}*(u32*)(m+24)=0;return 0;}
