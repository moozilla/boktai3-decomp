#include "global.h"
#define ResultMenu_DestroyGraphics sub_0801C5AC
void sub_082195E0(void *);void sub_08217EAC(void *);
int ResultMenu_DestroyGraphics(u8 *p){u8 *g=p+0x144;int i;for(i=14;i>=0;g+=96,i--)sub_082195E0(g);g=p+0x6e4;for(i=3;i>=0;g+=40,i--)sub_08217EAC(g);}
