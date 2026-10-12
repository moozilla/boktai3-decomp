#include "global.h"
int sub_08011578(void*,int,int);
#define GlyphTable_EncodeName sub_080115A4
int GlyphTable_EncodeName(u8*m){u8*p=m+24;int step=2,i;for(i=0;i<24;i++,p+=step){int first=p[0],second=p[1];unsigned value=sub_08011578(*(void**)(m+0x7c),first,second);if(value>63)return -1;m[i]=value;}return 0;}
