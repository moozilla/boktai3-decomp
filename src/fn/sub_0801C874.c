// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define ResultGame_TrackTarget sub_0801C874
extern u8 gUnk_0824DB8A[],gUnk_0824DB7A[];
void sub_0801C16C(void*);void sub_0801C17C(void*);void sub_0801C1A0(void*);void sub_0801C190(void*);void sub_0822B2F8(int);void sub_0822B358(int);void sub_0801C6CC(void*);void sub_0801C760(void*);void sub_0801C69C(void*,void*);void sub_0801CC24(void*);
#define I(o) (*(int*)(p+(o)))
#define H(o) (*(s16*)(p+(o)))
void ResultGame_TrackTarget(u8*p){int speed=gUnk_0824DB8A[p[0xae]];
 switch(p[0xaf]) {
 case 0:{int zero;int *start=(int*)(p+0x8c);asm("" : : "r"(start));{register int z asm("r2")=0;asm("" : "+r"(z));zero=z;}I(0x8c)=zero;I(0x90)=zero;p[0xb2]=zero;I(0x9c)=zero;I(0x94)=gUnk_0824DB7A[p[0xae]*2]<<5;I(0x98)=(gUnk_0824DB7A+1)[p[0xae]*2]<<5;++p[0xaf];H(0xaa)=zero;H(0xa8)=zero;p[0xb3]=32;p[0xbf]=255;sub_0801C16C(p);sub_0801C17C(p);sub_0801C1A0(p);sub_0822B2F8(0x4b1);break;}
 case 1:I(0x8c)+=176;I(0x9c)=I(0x8c);if(I(0x9c)>=I(0x98)){int zero;register u8 *state asm("r2")=p+0xaf;asm("" : "+r"(state));zero=0;asm("" : "+r"(zero));*state=4;H(0xa8)=zero;}break;
 case 2:I(0x9c)+=speed;sub_0801C6CC(p);if((H(0xa8)&15)==0)sub_0801C760(p);if(I(0x9c)>=I(0x98)){++p[0xaf];H(0xaa)=30;}break;
 case 3:sub_0801C6CC(p);if((H(0xa8)&15)==0)sub_0801C760(p);if(H(0xaa)<=0)++p[0xaf];break;
 case 4:I(0x9c)-=speed;sub_0801C6CC(p);if((H(0xa8)&15)==0)sub_0801C760(p);if(I(0x9c)<=I(0x94)){++p[0xaf];H(0xaa)=10;}break;
 case 5:sub_0801C6CC(p);if((H(0xa8)&15)==0)sub_0801C760(p);if(H(0xaa)<=0)p[0xaf]=2;break;
 case 6:I(0x8c)-=176;if(I(0x8c)<=0){I(0x8c)=0;sub_0801C69C(p,sub_0801CC24);if(p[0xc4]==1)p[0xc4]=2;}break;
 }
 {int fade=32-Div(H(0xa8)<<5,1200);asm("" : "+r"(fade));p[0xb3]=fade;}--H(0xaa);if(++H(0xa8)>1200)H(0xa8)=1200;
 if(p[0xaf]!=6 && (p[0xc4]==1 || H(0xa8)>1199)){sub_0822B358(0x3ed);sub_0822B358(0x3ee);p[0xaf]=6;p[0xb3]=0;sub_0801C190(p);}
}
