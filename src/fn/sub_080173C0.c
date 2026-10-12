#include "global.h"
extern void*gUnk_0200007C;int sub_0821ABA8(int,int);void*sub_08219FBC(int,int);void sub_0821A04C(void*,void*,void*);int sub_08017270(void*);int sub_080172D8(void*);int sub_080172EC(void*,int);void sub_0821A0C0(void*);
#define ScanlineWave_Create sub_080173C0
void*ScanlineWave_Create(int id){void*p;if(gUnk_0200007C)return gUnk_0200007C;if(sub_0821ABA8('a',0))p=sub_08219FBC(11,628);else p=sub_08219FBC(9,628);if(p){sub_0821A04C(p,sub_08017270,sub_080172D8);if(sub_080172EC(p,(u16)id)<0){sub_0821A0C0(p);return 0;}}return p;}
