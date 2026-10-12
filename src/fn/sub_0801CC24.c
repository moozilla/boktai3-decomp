// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define ResultGame_TimedPress sub_0801CC24
extern int gUnk_0824DB94[];extern void *gUnknown_02000580;
void sub_0801C17C(void*);void sub_0801C1C4(void*);void sub_0815BE5C(void*,int);void sub_0801C1E8(void*,int,int,void*);void sub_0822B2F8(int);void sub_0801CBCC(void*);void sub_0801CAF0(void*);void sub_0801C16C(void*);void sub_0801C190(void*);void sub_0801C69C(void*,void*);void sub_0801C874(void*);
#define I(o) (*(int*)(p+(o)))
#define H(o) (*(s16*)(p+(o)))
void ResultGame_TimedPress(u8*p){switch(p[0xaf]){
 case 0:I(0x8c)=0;p[0xb3]=0;I(0x9c)=1536;sub_0801C17C(p);sub_0801C1C4(p);{register u8 *state asm("r1")=p+0xb0;int minus;asm("" : : "r"(state));*state=255;p[0xb1]=0;++p[0xaf];{register u8 *out asm("r1")=p+0xbf;asm("" : : "r"(out));minus=-1;asm("" : "+r"(minus));*out=minus;}}sub_0815BE5C(gUnknown_02000580,1);sub_0801C1E8(p,0x35,0,0);sub_0822B2F8(0x4b2);break;
 case 1:{int zero;int zeroByte;int next=p[0xaf]+1;s16 *timer;asm("" : "+r"(next));zero=0;asm("" : "+r"(zero));p[0xaf]=next;timer=(s16*)(p+0xaa);asm("" : : "r"(timer));zeroByte=0;asm("" : "+r"(zeroByte));*timer=60;H(0xa8)=zero;p[0xb0]=255;p[0xb1]=zeroByte;break;}
 case 2:if(H(0xaa)<=0)++p[0xaf];break;
 case 3:I(0x8c)+=1024;p[0xb0]=255;if(I(0x8c)>5631){I(0x8c)=5632;{register u8 *state asm("r2")=p+0xaf;int next;int zero;asm("" : "+r"(state));next=*state+1;asm("" : "+r"(next));zero=0;asm("" : "+r"(zero));*state=next;p[0xb0]=zero;}}break;
 case 4:I(0x8c)-=gUnk_0824DB94[I(0x50)];sub_0801CBCC(p);if(I(0x8c)<=0){{int signedState=(s8)p[0xb0];asm("" : "+r"(signedState));if(signedState==0){sub_0801CAF0(p);sub_0801C16C(p);sub_0801C1E8(p,0x34,0,0);p[0xb1]=1;}}p[0xb0]=255;I(0x8c)=0;p[0xaf]=3;if(p[0xc4]==1||H(0xa8)>899){sub_0801C69C(p,sub_0801C874);sub_0801C190(p);sub_0801C16C(p);if(p[0xc4]==1)p[0xc4]=2;else{sub_0801C1E8(p,0x36,0,0);sub_0815BE5C(gUnknown_02000580,0);}}}break;
 }
 p[0xb3]=Div(H(0xa8)<<5,900);++H(0xa8);--H(0xaa);if(H(0xa8)>900)H(0xa8)=900;
}
