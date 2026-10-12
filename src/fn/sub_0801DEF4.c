// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define ResultTutorial_Initialize sub_0801DEF4
typedef struct {u32 words[8];} Resource;
Resource *sub_0821A520(int,int);void sub_082196C4(void*,void*);int sub_0801DEBC(void*);void sub_0801BCA8(void*);void sub_0801B534(void*,void*);void sub_0801DC7C(void*);
extern u32 gUnk_0300523C;void sub_0822AE7C(int);
int ResultTutorial_Initialize(u8*p,void *owner){Resource*r;int four,zero;*(void**)(p+0x78)=owner;r=sub_0821A520(0xcb05,0x530d);if(!r)return -1;*(Resource*)(p+0x104)=*r;sub_082196C4(p+0x104,r);r=sub_0821A520(0xcb05,0x5d04);if(!r)return -1;*(Resource*)(p+0x124)=*r;sub_082196C4(p+0x124,r);if(sub_0801DEBC(p)<0)return -1;{u8 *out=p+0xae;asm("" : : "r"(out));}zero=0;asm("" : "+r"(zero));four=4;asm("" : "+r"(four));p[0xae]=four;p[0xb4]=zero;p[0xb6]=zero;*(int*)(p+0x50)=3;sub_0801BCA8(p);sub_0801B534(p,sub_0801DC7C);sub_0822AE7C(0xaa);gUnk_0300523C|=four;return 0;}
