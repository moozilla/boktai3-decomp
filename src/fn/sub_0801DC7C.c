#include "global.h"
#define ResultTutorial_Open sub_0801DC7C
int sub_0801B49C(int,int);void sub_0801B568(void);void sub_08030C84(void*);void sub_08030F20(int,int,int,int);void sub_080311B0(int);void sub_08030B78(int);void sub_0801BD44(void*);void sub_08030DBC(int);void sub_082279A8(int,int,int,int,int,int,int);void sub_0801B534(void*,void*);void sub_0801DD40(void*);
typedef struct{u32 pad[2],flags;} Graphic;
void ResultTutorial_Open(u8*p){switch(p[0xaf]){case 0:p[0xb7]=sub_0801B49C(14,1);sub_0801B568();sub_08030C84(*(void**)(p+0x74));sub_08030F20(1,1,30,6);sub_080311B0(0x81b);sub_08030B78(1);sub_0801BD44(p);sub_08030DBC(p[0xb6]);sub_082279A8(0,5,4,4,4,0xffff,0);++p[0xaf];*(u16*)(p+0xaa)=0;break;case 1:if(++*(s16*)(p+0xaa)>31){((Graphic*)(p+0x684))->flags&=~1;p[0xb5]=16;sub_0801B534(p,sub_0801DD40);}break;}}
