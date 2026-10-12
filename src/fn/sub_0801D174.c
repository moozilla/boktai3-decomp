#include "global.h"
#define ResultTutorial_ShowPrompt sub_0801D174
void sub_08030DBC(int);int sub_0801D104(void*);
typedef struct{u32 pad[2],flags;} Graphic;
int ResultTutorial_ShowPrompt(u8*p){switch(p[0xaf]){case 0:sub_08030DBC(p[0xb6]);++p[0xaf];*(u16*)(p+0xaa)=0;case 1:if(sub_0801D104(p)){((Graphic*)(p+0x684))->flags&=~1;return 1;}break;}return 0;}
